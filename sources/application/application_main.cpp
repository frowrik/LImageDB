#include "application_header.h"

void Run_DrawDX11() { 
    
}

bool Run_Init() {
    database.open( "database/database.db" );
    database.clear();
    database.testfill();

    if ( !ui::init() ) return false;

    return true; 
}

void Run_Free() {

    ui::free();

    database.close();
}

bool Run_Draw() {
    ui::draw();

    ImGui::ShowDemoWindow();

    return true;
}

int main() {
    srand(::time(nullptr));
    Run_Run(1600,1000);
    return 0;
}
