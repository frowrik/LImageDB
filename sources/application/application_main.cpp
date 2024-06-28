#include "application_header.h"

#include "sql-gui.h"

sqlite3* db = nullptr;
sqlview  dbview;

void Run_DrawDX11() { 
    
}


//void create_table_() {
//    if ( err_msg ) {
//        sqlite3_free( err_msg );
//        err_msg = NULL;
//    }
//
//    snprintf( query, sizeof( query ), "%s", editor.GetText().c_str() );
//
//    char** new_result = NULL;
//    int    new_rows   = 0;
//    int    new_cols   = 0;
//    rc                = sqlite3_get_table( db, query, &new_result, &new_rows, &new_cols, &err_msg );
//    if ( rc != SQLITE_OK ) {
//        fprintf( stderr, "SQL error: %s\n", err_msg );
//        if ( new_result ) { sqlite3_free_table( new_result ); }
//    } else if ( new_cols > 64 ) {
//        fprintf( stderr, "Error %d > 64 columns\n", new_cols );
//        if ( new_result ) { sqlite3_free_table( new_result ); }
//    } else {
//        if ( result ) {
//            sqlite3_free_table( result );
//            result = NULL;
//        }
//        result      = new_result;
//        result_rows = new_rows;
//        result_cols = new_cols;
//    }
//}

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
