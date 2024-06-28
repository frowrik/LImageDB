/*
SQLite database query tool made with Dear ImGui 
https://github.com/jminor/sql-gui/tree/main
MIT license 
*/

#include <sqlite3.h>
#include "ImGuiColorTextEdit/TextEditor.h"

class sqlview{
public:
    sqlview() {
        snprintf( query, sizeof( query ), "%s", "select * from sqlite_master" );

		auto lang = TextEditor::LanguageDefinition::SQL();
		editor.SetLanguageDefinition(lang);
		editor.SetShowWhitespaces(false);
		TextEditor::Palette palette = TextEditor::GetLightPalette();
		// disable the current line highlight, by choosing transparent colors for it.
		palette[(int)TextEditor::PaletteIndex::CurrentLineFill] = 0x00000000;
		palette[(int)TextEditor::PaletteIndex::CurrentLineFillInactive] = 0x00000000;
		palette[(int)TextEditor::PaletteIndex::CurrentLineEdge] = 0x00000000;
		editor.SetPalette(palette);
        editor.SetText( query );
	}	

	void draw( sqlite3 *db ){
        if ( !db ) return;

        ImGuiIO &io = ImGui::GetIO();
        ImGuiStyle &style    = ImGui::GetStyle();

		bool do_query = false;

		if (ImGui::GetFrameCount()==1) do_query = true;

		//ImGui::SetNextWindowPos(ImVec2(0,0), ImGuiCond_Always);
		//ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);
        const ImGuiViewport *main_viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos( ImVec2( main_viewport->WorkPos.x + 0, main_viewport->WorkPos.y + 0 ), ImGuiCond_FirstUseEver );
        ImGui::SetNextWindowSize( ImVec2( 550, 680 ), ImGuiCond_FirstUseEver );

        ImGui::Begin( "Database", nullptr, ImGuiWindowFlags_NoDocking );

		if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None)) {

			if (ImGui::BeginTabItem("SQL")) {

				ImVec2 size(
					ImGui::GetContentRegionAvail().x - 100 - style.FramePadding.x,
					ImGui::GetTextLineHeight() * 15
					);
				editor.Render("SQL", size, true);
				ImVec2 bottom_corner = ImGui::GetItemRectMax();

				ImGui::SameLine();

				if (ImGui::BeginChild("Query Buttons", ImVec2(100,50))) {
					if (ImGui::Button("Run Query")) {
						do_query = true;
					}
					ImGui::Text("%s+Enter", io.ConfigMacOSXBehaviors ? "Cmd" : "Ctrl");
				}
				ImGui::EndChild();

				ImGuiIO& io = ImGui::GetIO();
				auto shift = io.KeyShift;
				auto ctrl = io.ConfigMacOSXBehaviors ? io.KeySuper : io.KeyCtrl;
				auto alt = io.ConfigMacOSXBehaviors ? io.KeyCtrl : io.KeyAlt;
				if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter), false)) {
					do_query = true;
				}

				auto cpos = editor.GetCursorPosition();
				auto selection = editor.GetSelectedText();
				char info_text[1024];
				if (selection.empty()) {
					snprintf(info_text, sizeof(info_text),
						"line %d/%d, column %d | %s",
						cpos.mLine + 1,
						editor.GetTotalLines(),
						cpos.mColumn + 1,
						editor.IsOverwrite() ? "Ovr" : "Ins");
				}else{
					snprintf(info_text, sizeof(info_text),
						"selected %d characters | %s",
						(int)selection.length(),
						editor.IsOverwrite() ? "Ovr" : "Ins");
				}

				ImVec2 pos = ImGui::GetCursorPos();
				ImGui::SetCursorPosX(bottom_corner.x - ImGui::CalcTextSize(info_text).x);
				ImGui::TextUnformatted(info_text);
				// ImGui::SetItemAllowOverlap();
				ImGui::SetCursorPos(pos);

				if (do_query) {
					if (err_msg) {
						sqlite3_free(err_msg);
						err_msg = NULL;
					}

					snprintf(query, sizeof(query), "%s", editor.GetText().c_str());

					char **new_result = NULL;
					int new_rows = 0;
					int new_cols = 0;
					rc = sqlite3_get_table(
						db,
						query,
						&new_result,
						&new_rows,
						&new_cols,
						&err_msg
						);
					if (rc != SQLITE_OK) {
						fprintf(stderr, "SQL error: %s\n", err_msg);
						if (new_result) {
							sqlite3_free_table(new_result);
						}
					}else if (new_cols>64) {
						fprintf(stderr, "Error %d > 64 columns\n", new_cols);
						if (new_result) {
							sqlite3_free_table(new_result);
						}
					}else{
						if (result) {
							sqlite3_free_table(result);
							result = NULL;
						}
						result = new_result;
						result_rows = new_rows;
						result_cols = new_cols;
					}
				}

				if (err_msg) {
					ImGui::Text("%s", err_msg);
				}

				if (result) {
					ImGui::Text("Result %d rows, %d cols", result_rows, result_cols);

					DisplayTable(result, result_rows, result_cols);
				}

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Tables")) {

				char **result = NULL;
				int rows = 0;
				int cols = 0;

				// which tables exist?
				rc = sqlite3_get_table(
					db,
					"select name from sqlite_master where type='table'",
					&result,
					&rows,
					&cols,
					&err_msg
					);
				if (rc != SQLITE_OK) {
					fprintf(stderr, "SQL error: %s\n", err_msg);
					if (result) {
						sqlite3_free_table(result);
					}
				}else{

					// pick a table
					static int selected_table_index = 0;
					ImGui::Combo("Table", &selected_table_index, &result[1], rows);

					static char filter[1024];
					ImGui::InputText("Filter", filter, sizeof(filter));

                    char q[256];
					if ( rows != 0 ) {
                        const char *where = filter;
                        if ( !strlen( where ) ) { where = "1=1"; }
                        snprintf( q, sizeof( q ), "select * from %s where %s", result[selected_table_index + 1], where );

                        // free the list of tables
                        sqlite3_free_table( result );
                    }

					// query the full contents of the table
					rc = sqlite3_get_table(
						db,
						q,
						&result,
						&rows,
						&cols,
						&err_msg
						);
					if (rc != SQLITE_OK) {
						fprintf(stderr, "SQL error: %s\n", err_msg);
						if (result) {
							sqlite3_free_table(result);
						}
					}else{
						ImGui::Text("%d rows, %d cols", rows, cols);

						DisplayTable(result, rows, cols);
						sqlite3_free_table(result);
					}
				}


				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Records")) {

				char **result = NULL;
				int rows = 0;
				int cols = 0;

				// which tables exist?
				rc = sqlite3_get_table(
					db,
					"select name from sqlite_master where type='table'",
					&result,
					&rows,
					&cols,
					&err_msg
					);
				if (rc != SQLITE_OK) {
					fprintf(stderr, "SQL error: %s\n", err_msg);
					if (result) {
						sqlite3_free_table(result);
					}
				}else{
                    if ( rows != 0 ) {
                        // pick a table
                        static int selected_table_index = 0;
                        ImGui::Combo( "Table", &selected_table_index, &result[1], rows );

                        static char filter[1024];
                        ImGui::InputText( "Filter", filter, sizeof( filter ) );

                        char        q[256];
                        const char *where = filter;
                        if ( !strlen( where ) ) { where = "1=1"; }
                        snprintf( q, sizeof( q ), "select * from %s where %s", result[selected_table_index + 1], where );

                        // free the list of tables
                        sqlite3_free_table( result );

                        // query the full contents of the table
                        rc = sqlite3_get_table( db, q, &result, &rows, &cols, &err_msg );
                        if ( rc != SQLITE_OK ) {
                            fprintf( stderr, "SQL error: %s\n", err_msg );
                            if ( result ) { sqlite3_free_table( result ); }
                        } else {

                            // Pick one record
                            static int record_index = 1;
                            if ( record_index > rows ) { record_index = 1; }

                            ImGui::AlignTextToFramePadding();
                            ImGui::Text( "Record %d of %d", record_index, rows );
                            ImGui::SameLine();
                            if ( ImGui::Button( "Prev" ) ) {
                                record_index--;
                                if ( record_index < 1 ) record_index = rows;
                            }
                            ImGui::SameLine();
                            if ( ImGui::Button( "Next" ) ) {
                                record_index++;
                                if ( record_index > rows ) record_index = 1;
                            }

                            ImGui::SliderInt( "Record Index", &record_index, 1, rows );

                            ImGuiTableFlags flags = 0 | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY;
                            if ( ImGui::BeginTable( "Record", 2, flags ) ) {
                                for ( int col = 0; col < cols; col++ ) {
                                    const char *column_name = result[col];
                                    const char *value       = result[record_index * cols + col];

                                    ImGui::TableNextRow();

                                    ImGui::TableSetColumnIndex( 0 );
                                    ImGui::AlignTextToFramePadding();
                                    ImGui::TextUnformatted( column_name );

                                    ImGui::TableSetColumnIndex( 1 );
                                    ImGui::AlignTextToFramePadding();
                                    ImGui::TextUnformatted( value );
                                }
                                ImGui::EndTable();
                            }

                            sqlite3_free_table( result );
                        }
                    }
				}


				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}

		ImGui::End();
	}

private:
	TextEditor editor;

    char       query[1024];
    char     **result      = NULL;
    int        result_rows = 0;
    int        result_cols = 0;
    char      *err_msg     = NULL;
    int        rc          = 0;

	void DisplayTable(char **result, int rows, int cols) {
		ImGuiTableFlags flags = 0
		| ImGuiTableFlags_Borders
		| ImGuiTableFlags_RowBg
		| ImGuiTableFlags_Resizable
		| ImGuiTableFlags_ScrollY
		;

		if (cols>0 && ImGui::BeginTable("Result", cols, flags)) {

			for (int col=0; col<cols; col++) {
				ImGui::TableSetupColumn(result[col]);
			}
			ImGui::TableSetupScrollFreeze(0, 1);
			ImGui::TableHeadersRow();

			for (int row=0; row<rows; row++) {
				ImGui::TableNextRow();
				for (int col=0; col<cols; col++) {
					ImGui::TableSetColumnIndex(col);
					const char *text = result[(row+1)*cols+col];
					if (text == NULL) {
						ImGui::TextDisabled("<NULL>");
					}else{
						ImGui::TextUnformatted(text);
					}
				}
			}

			ImGui::EndTable();
		}
	}
};