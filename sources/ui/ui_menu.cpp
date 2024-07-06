#include "ui_header.h"

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
