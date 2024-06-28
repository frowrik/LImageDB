#include "application_header.h"

#include "sql-gui.h"

sqlite3* db = nullptr;
sqlview  dbview;

void Run_DrawDX11() { 
    
}

bool Run_Init() {
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();
    io.Fonts->AddFontFromFileTTF( "Inter-Medium.ttf", 18.0f );
    

    database.open( "database/database.db" );
    database.clear();
    database.testfill();

    return true; 
}

void Run_Free() {
    database.close();
}

bool Run_Draw() {
    dbview.draw( database.db );

    {
        const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos( ImVec2( main_viewport->WorkPos.x + 100, main_viewport->WorkPos.y + 100 ), ImGuiCond_FirstUseEver );
        ImGui::SetNextWindowSize( ImVec2( 300, 200 ), ImGuiCond_FirstUseEver );
        ImGui::Begin( "Test", nullptr, ImGuiWindowFlags_NoDocking );

        if ( ImGui::Button( "remove file 3" ) ) {
            database.sql_execute_noresult( "DELETE FROM files WHERE id=3;" );
            ;
        }

        if ( ImGui::Button( "remove file link3" ) ) { database.table_file_set_tag_remove()
            ;
        }

        ImGui::Separator();
        {
            auto count_all_record = database.table_files_get_count();
            
            static ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_ScrollY;
            static ImGuiTableColumnFlags flagsColumn = ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_NoReorder;
            if ( ImGui::BeginTable( "table_sorting", 9, flags, ImVec2( 0.0f, 0.0 ), 0.0f ) ) {
                ImGui::TableSetupColumn( "id", flagsColumn, 0.0f, 1 );
                ImGui::TableSetupColumn( "shortname", flagsColumn, 0.0f, 2 );
                ImGui::TableSetupColumn( "filetype_id", flagsColumn, 0.0f, 3 );
                ImGui::TableSetupColumn( "path_id", flagsColumn, 0.0f, 4 );
                ImGui::TableSetupColumn( "path_to_file", flagsColumn, 0.0f, 5 );
                ImGui::TableSetupColumn( "filesize", flagsColumn, 0.0f, 6 );
                ImGui::TableSetupColumn( "filetimewrite", flagsColumn, 0.0f, 7 );
                ImGui::TableSetupColumn( "md5", flagsColumn, 0.0f, 8 );
                ImGui::TableSetupColumn( "name_miniature", flagsColumn, 0.0f, 9 );
                ImGui::TableSetupScrollFreeze( 0, 1 );  // Make row always visible
                ImGui::TableHeadersRow();
                database_file    records[256];

                ImGuiListClipper clipper;
                clipper.Begin( count_all_record );
                while ( clipper.Step() ) {
                    uint32_t      offset = clipper.DisplayStart;
                    uint32_t      neadcount = clipper.DisplayEnd - clipper.DisplayStart;
                    while ( offset < clipper.DisplayEnd ) {
                        uint32_t records_count = std::min( std::size( records ), (size_t)neadcount );
                        if ( !database.table_files_select( offset, records_count, records ) ) break;
                        if ( records_count == 0 ) break;

                        for ( uint32_t i = 0; i < records_count; i++ ) {
                            const database_file& file_ = records[i];
                            ImGui::PushID( file_.id );
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text( "%lld", file_.id );
                            ImGui::TableNextColumn();
                            ImGui::TextUnformatted( file_.shortname.c_str() );
                            ImGui::TableNextColumn();
                            ImGui::Text( "%d", file_.filetype_id );
                            ImGui::TableNextColumn();
                            ImGui::Text( "%d", file_.path_id );
                            ImGui::TableNextColumn();
                            ImGui::TextUnformatted( file_.path_to_file.c_str() );
                            ImGui::TableNextColumn();
                            ImGui::Text( "%lld", file_.filesize );
                            ImGui::TableNextColumn();
                            ImGui::Text( "%lld", file_.filetimewrite );
                            ImGui::TableNextColumn();
                            ImGui::TextUnformatted( file_.md5.c_str() );
                            ImGui::TableNextColumn();
                            ImGui::TextUnformatted( file_.name_miniature.c_str() );
                            ImGui::PopID();
                        }

                        offset += records_count;
                    }
                }
                ImGui::EndTable();
            }
        }
        ImGui::Separator();

        ImGui::End();
    }

    //ImGui::ShowDemoWindow();
	
    return true;
}

int main() {
    srand(::time(nullptr));
    Run_Run(1600,1000);
    return 0;
}
