#include "ui_header.h"

namespace ui {
    namespace browser {


    }  // namespace browser
}  // namespace ui

void ui::browser::init() {}

void ui::browser::free() {}

void ui::browser::draw() {
    auto window = ImGui::GetCurrentWindow();

    static int32_t          global_offset = 0;

    
    static int32_t elementsperdisplay = 100;

    static ImGuiListClipper clipper;  // static nead
    clipper.Begin( elementsperdisplay );

    while ( clipper.Step() ) {

        uint32_t offset    = clipper.DisplayStart;
        uint32_t neadcount = clipper.DisplayEnd - clipper.DisplayStart;
        
        for ( uint32_t i = 0; i < neadcount; i++ ) { 
            
            ImGui::Text( "line %d", offset + i + global_offset );
        }
    }
    clipper.End();

    
    float          ScrollSizeY     = window->ScrollbarSizes.y;
    float          ScrollMaxY     = ImGui::GetScrollMaxY();
    float          ScrollY        = ImGui::GetScrollY();
    float          ScrollYPercent  = ( ScrollY - ScrollSizeY / 2 ) / ScrollMaxY;
    if ( ScrollYPercent > 0.75 && !ImGui::IsMouseDragging( ImGuiMouseButton_Left ) ) {
        ImGui::SetScrollY( window, 0.80 * ( ScrollMaxY - ScrollSizeY ) );
        global_offset += elementsperdisplay / 2;
        if ( global_offset + elementsperdisplay > 10000 ) 
            global_offset = 10000 - elementsperdisplay;
    } else if ( ScrollYPercent < 0.15 && !ImGui::IsMouseDragging( ImGuiMouseButton_Left ) ) {
        ImGui::SetScrollY( window, 0.20 * (ScrollMaxY - ScrollSizeY) );
        global_offset -= elementsperdisplay / 2;
        if ( global_offset < 0 ) 
            global_offset = 0;
    } 

    return;

    if ( ImGui::Button( "remove file 3" ) ) {
        database.sql_execute_noresult( "DELETE FROM files WHERE id=3;" );
        ;
    }

    if ( ImGui::Button( "remove file link3" ) ) { database.table_file_set_tag_remove(); }

    ImGui::Separator();

    {
        auto count_all_record = database.table_files_get_count();

        static ImGuiTableFlags       flags       = ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_ScrollY;
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
            database_file records[256];

            ImGuiListClipper clipper;  // static nead
            clipper.Begin( count_all_record );
            while ( clipper.Step() ) {
                uint32_t offset    = clipper.DisplayStart;
                uint32_t neadcount = clipper.DisplayEnd - clipper.DisplayStart;
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
            clipper.End();
            ImGui::EndTable();
        }
    }
}
