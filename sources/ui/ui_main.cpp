#include "ui_header.h"

#include "sql-gui.h"

sqlview dbview;

namespace ui {

    ImGuiID RightDockId = 0;
    ImGuiID CentralDockId = 0;
    void    build_dock();

    void draw_menu();

    void build_style();


   
}  // namespace ui

bool ui::init() {
    //
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();
    io.Fonts->AddFontFromFileTTF( "Inter-Medium.ttf", 18.0f );

    build_style();

    return true;
    ;
}

void ui::free() {
    ;
    ;
}

void ui::draw() {

    // fix style
    ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
    ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );
    ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0.0f, 0.0f ) );

    // def flags
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    window_flags |= ImGuiWindowFlags_NoBackground;  // disable bg

    // set main viewport
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos( viewport->WorkPos );
    ImGui::SetNextWindowSize( viewport->WorkSize );
    ImGui::SetNextWindowViewport( viewport->ID );
    ImGui::Begin( "MainPanel", nullptr, window_flags );
    {
        ImGui::DockSpace( ImGui::GetID( "MainPanelDock" ), ImVec2( 0.0f, 0.0f ), ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_HiddenTabBar );

        build_dock();

        draw_menu();
    }
    ImGui::End();

    ImGui::PopStyleVar( 3 );

    ImGuiWindowFlags window_flags2 = 
        ImGuiWindowFlags_UnsavedDocument |
        ImGuiWindowFlags_NoTitleBar | 
        ImGuiWindowFlags_NoCollapse | 
        ImGuiWindowFlags_AlwaysAutoResize | 
        ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoMove;
    if ( ImGui::Begin( "Contents", nullptr, window_flags2 ) ) {

        if ( ImGui::BeginTabBar( "##ContentsBar", ImGuiTabBarFlags_None ) ) {

            if ( ImGui::BeginTabItem( "Browser" ) ) {
                ImGui::Text( "Browser" );

                if ( ImGui::Button( "remove file 3" ) ) {
                    database.sql_execute_noresult( "DELETE FROM files WHERE id=3;" );
                    ;
                }

                if ( ImGui::Button( "remove file link3" ) ) { database.table_file_set_tag_remove(); }

                ImGui::Separator();
                {
                    auto count_all_record = database.table_files_get_count();

                    static ImGuiTableFlags       flags       = ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_ScrollY;
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
                ImGui::Separator();

                ImGui::EndTabItem();
            }

            if ( ImGui::BeginTabItem( "SQLDebug" ) ) {
                dbview.draw( database.db->getHandle() );
                ImGui::EndTabItem();
            }

            if ( ImGui::BeginTabItem( "Log" ) ) {
                ImGui::Text( "Log" );
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        ImGui::End();
    }

    if ( ImGui::Begin( "Property", nullptr, window_flags2 ) ) {
        ImGui::Text( "Property" );
        ImGui::End();
    }
}

void ui::draw_menu() {
    if ( !ImGui::BeginMenuBar() ) return;

    if ( ImGui::BeginMenu( "File" ) ) {
        if ( ImGui::MenuItem( "New project" ) ) {}
        if ( ImGui::MenuItem( "Open project" ) ) {}
        if ( ImGui::MenuItem( "Save project" ) ) {}
        if ( ImGui::MenuItem( "Save project as..." ) ) {}
        if ( ImGui::MenuItem( "Quit" ) ) {}
        ImGui::EndMenu();
    }

    ImGui::EndMenuBar();
}

void ui::build_dock() {
    //
    static bool is_first = false;
    if ( is_first ) return;
    is_first = true;

    //
    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGuiID dockspace_id = ImGui::GetID( "MainPanelDock" );
    ImGui::DockBuilderRemoveNode( dockspace_id );
    ImGui::DockBuilderAddNode( dockspace_id, ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_DockSpace );
    ImGui::DockBuilderSetNodeSize( dockspace_id, viewport->Size );

    ImGuiID temp;
    RightDockId = ImGui::DockBuilderSplitNode( dockspace_id, ImGuiDir_Right, 0.3f, nullptr, &CentralDockId );

    ImGui::DockBuilderDockWindow( "Property", RightDockId );
    ImGui::DockBuilderDockWindow( "Contents", CentralDockId );
    //ImGui::DockBuilderDockWindow( "SQLDebug", CentralDockId );
    //ImGui::DockBuilderDockWindow( "Log", CentralDockId );
    ImGui::DockBuilderFinish( dockspace_id );
}

void ui::build_style() {
    ImGuiStyle& style                           = ImGui::GetStyle();
    ImGuiIO&    io                              = ImGui::GetIO();
    style.Alpha                                 = 1.0f;
    style.FrameRounding                         = 3.0f;
    style.Colors[ImGuiCol_Text]                 = ImVec4( 1.000f, 1.000f, 1.000f, 1.000f );
    style.Colors[ImGuiCol_TextDisabled]         = ImVec4( 0.000f, 0.543f, 0.983f, 1.000f );
    style.Colors[ImGuiCol_WindowBg]             = ImVec4( 0.000f, 0.000f, 0.000f, 0.895f );
    style.Colors[ImGuiCol_ChildBg]              = ImVec4( 0.00f, 0.00f, 0.00f, 0.00f );
    style.Colors[ImGuiCol_PopupBg]              = ImVec4( 0.07f, 0.07f, 0.09f, 1.00f );
    style.Colors[ImGuiCol_Border]               = ImVec4( 0.184f, 0.878f, 0.000f, 0.500f );
    style.Colors[ImGuiCol_BorderShadow]         = ImVec4( 1.00f, 1.00f, 1.00f, 0.10f );
    style.Colors[ImGuiCol_FrameBg]              = ImVec4( 0.160f, 0.160f, 0.160f, 0.315f );
    style.Colors[ImGuiCol_FrameBgHovered]       = ImVec4( 0.160f, 0.160f, 0.160f, 0.315f );
    style.Colors[ImGuiCol_FrameBgActive]        = ImVec4( 0.210f, 0.210f, 0.210f, 0.670f );
    style.Colors[ImGuiCol_TitleBg]              = ImVec4( 0.026f, 0.597f, 0.000f, 1.000f );
    style.Colors[ImGuiCol_TitleBgCollapsed]     = ImVec4( 0.099f, 0.315f, 0.000f, 0.000f );
    style.Colors[ImGuiCol_TitleBgActive]        = ImVec4( 0.026f, 0.597f, 0.000f, 1.000f );
    style.Colors[ImGuiCol_ScrollbarBg]          = ImVec4( 0.000f, 0.000f, 0.000f, 0.801f );
    style.Colors[ImGuiCol_ScrollbarGrab]        = ImVec4( 0.238f, 0.238f, 0.238f, 1.000f );
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4( 0.238f, 0.238f, 0.238f, 1.000f );
    style.Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4( 0.004f, 0.381f, 0.000f, 1.000f );
    style.Colors[ImGuiCol_CheckMark]            = ImVec4( 0.009f, 0.845f, 0.000f, 1.000f );
    style.Colors[ImGuiCol_SliderGrab]           = ImVec4( 0.139f, 0.508f, 0.000f, 1.000f );
    style.Colors[ImGuiCol_SliderGrabActive]     = ImVec4( 0.139f, 0.508f, 0.000f, 1.000f );
    style.Colors[ImGuiCol_Button]               = ImVec4( 0.000f, 0.000f, 0.000f, 0.400f );
    style.Colors[ImGuiCol_ButtonHovered]        = ImVec4( 0.000f, 0.619f, 0.014f, 1.000f );
    style.Colors[ImGuiCol_ButtonActive]         = ImVec4( 0.06f, 0.53f, 0.98f, 1.00f );
    style.Colors[ImGuiCol_Header]               = ImVec4( 0.26f, 0.59f, 0.98f, 0.31f );
    style.Colors[ImGuiCol_HeaderHovered]        = ImVec4( 0.26f, 0.59f, 0.98f, 0.80f );
    style.Colors[ImGuiCol_HeaderActive]         = ImVec4( 0.26f, 0.59f, 0.98f, 1.00f );
    style.Colors[ImGuiCol_ResizeGrip]           = ImVec4( 0.000f, 1.000f, 0.221f, 0.597f );
    style.Colors[ImGuiCol_ResizeGripHovered]    = ImVec4( 0.26f, 0.59f, 0.98f, 0.67f );
    style.Colors[ImGuiCol_ResizeGripActive]     = ImVec4( 0.26f, 0.59f, 0.98f, 0.95f );
    style.Colors[ImGuiCol_PlotLines]            = ImVec4( 0.39f, 0.39f, 0.39f, 1.00f );
    style.Colors[ImGuiCol_PlotLinesHovered]     = ImVec4( 1.00f, 0.43f, 0.35f, 1.00f );
    style.Colors[ImGuiCol_PlotHistogram]        = ImVec4( 0.90f, 0.70f, 0.00f, 1.00f );
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4( 1.00f, 0.60f, 0.00f, 1.00f );
    style.Colors[ImGuiCol_TextSelectedBg]       = ImVec4( 0.26f, 0.59f, 0.98f, 0.35f );
    style.ItemSpacing                           = ImVec2( 10, 8 );
    style.FramePadding                          = ImVec2( 6, 4 );
    style.ItemInnerSpacing                      = ImVec2( 5, 4 );
    style.ScrollbarSize                         = 16.f;
    style.GrabMinSize                           = 8.f;
    style.WindowBorderSize                      = 1.f;
    style.FrameBorderSize                       = 1.f;
    style.WindowRounding                        = 0.f;
    style.FrameRounding                         = 1.f;
    style.TabRounding                           = 0.f;
    style.AntiAliasedLines                      = true;
    style.AntiAliasedFill                       = true;
    style.Colors[ImGuiCol_TabHovered]           = ImVec4( 0.26f, 0.59f, 0.38f, 0.80f );
    style.Colors[ImGuiCol_Tab]                  = ImVec4( 0.11f, 0.22f, 0.00f, 0.86f );
    style.Colors[ImGuiCol_TabSelected]          = ImVec4( 0.15f, 0.52f, 0.12f, 1.00f );
    style.Colors[ImGuiCol_TabSelectedOverline]  = ImVec4( 0.26f, 0.59f, 0.05f, 0.82f );
    style.Colors[ImGuiCol_Header]               = ImVec4( 0.00f, 0.59f, 0.47f, 0.31f );
    style.Colors[ImGuiCol_HeaderHovered]        = ImVec4( 0.26f, 0.59f, 0.02f, 0.98f );
    style.Colors[ImGuiCol_WindowBg]             = ImVec4( 0.00f, 0.02f, 0.11f, 0.89f );
    style.Colors[ImGuiCol_Border]               = ImVec4( 0.18f, 0.88f, 0.00f, 0.26f );
    style.Colors[ImGuiCol_Separator]            = ImVec4( 0.43f, 0.43f, 0.50f, 0.80f );
    style.Colors[ImGuiCol_TableBorderStrong]    = ImVec4( 0.31f, 0.31f, 0.35f, 0.79f );
    style.Colors[ImGuiCol_TableRowBgAlt]        = ImVec4( 0.73f, 0.92f, 1.00f, 0.10f );
}