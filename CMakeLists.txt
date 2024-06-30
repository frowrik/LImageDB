cmake_minimum_required(VERSION 3.16)

project(LImageDB_project C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/project/intermediates")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/project/intermediates")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build/$<0:>")

set(DEPS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/deps)
include_directories(${DEPS_DIRECTORY})

set(SOURCES_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/sources)
include_directories(${SOURCES_DIRECTORY})
#######################################
macro(AutoGroupPCHFiles projname) 
    if(MSVC)
        source_group("CMake PCH" FILES
            "${PROJECT_BINARY_DIR}/CMakeFiles/${projname}.dir/cmake_pch.hxx"
            "${PROJECT_BINARY_DIR}/CMakeFiles/${projname}.dir/Debug/cmake_pch.hxx"
            "${PROJECT_BINARY_DIR}/CMakeFiles/${projname}.dir/Release/cmake_pch.hxx"
            "${PROJECT_BINARY_DIR}/CMakeFiles/${projname}.dir/MinSizeRel/cmake_pch.hxx"
            "${PROJECT_BINARY_DIR}/CMakeFiles/${projname}.dir/RelWithDebInfo/cmake_pch.hxx"
            "${PROJECT_BINARY_DIR}/CMakeFiles/${projname}.dir/cmake_pch.cxx")
    endif()
endmacro()

#######################################
# SDL3
set(SDL_SHARED OFF)
set(SDL_STATIC ON)
set(SDL_TEST   OFF)
set(SDL3_DISABLE_INSTALL    ON)
set(SDL_DISABLE_UNINSTALL  ON)
set(SDL_TEST_LIBRARY OFF)

add_subdirectory("${DEPS_DIRECTORY}/SDL" SDL)
include_directories("${DEPS_DIRECTORY}/SDL/include/SDL3")
set(sdl_sdl_libs "SDL3-static")
set_target_properties("SDL3-static" PROPERTIES FOLDER "deps")

#######################################
# imgui
include_directories("${DEPS_DIRECTORY}/imgui-docking")
file(GLOB IMGUI_SOURCES_FILES  "${DEPS_DIRECTORY}/imgui-docking/*.cpp" "${DEPS_DIRECTORY}/imgui-docking/*.h") 
file(GLOB IMGUIAddon_SOURCES_FILES  "${DEPS_DIRECTORY}/imgui-docking/ImGuiColorTextEdit/*.cpp" "${DEPS_DIRECTORY}/imgui-docking/ImGuiColorTextEdit/*.h") 
add_library("import_imgui" STATIC ${IMGUI_SOURCES_FILES} ${IMGUIAddon_SOURCES_FILES})
source_group("sources"  FILES  ${IMGUI_SOURCES_FILES})
source_group("sources/addons"  FILES  ${IMGUIAddon_SOURCES_FILES})
set_target_properties("import_imgui" PROPERTIES FOLDER "deps")

#######################################
# nativefiledialog
add_subdirectory("${DEPS_DIRECTORY}/nativefiledialog-extended" nativefiledialog)
set_target_properties("nfd" PROPERTIES FOLDER "deps")

#######################################
# sqlite
include_directories("${DEPS_DIRECTORY}/sqlite")
file(GLOB SQLLITE_SOURCES_FILES  "${DEPS_DIRECTORY}/sqlite/*.c" "${DEPS_DIRECTORY}/sqlite/*.h" )  
add_library("sqlite3" STATIC ${SQLLITE_SOURCES_FILES} )
source_group("sources"  FILES  ${SQLLITE_SOURCES_FILES})
set_target_properties("sqlite3" PROPERTIES FOLDER "deps")

#######################################
# SQLiteCpp
add_subdirectory("${DEPS_DIRECTORY}/SQLiteCpp" SQLiteCpp)
set_target_properties("SQLiteCpp" PROPERTIES FOLDER "deps")

#######################################
# import_stb
include_directories("${SOURCES_DIRECTORY}/import_stb")
file(GLOB STB_SOURCES_FILES  "${SOURCES_DIRECTORY}/import_stb/*.cpp" "${SOURCES_DIRECTORY}/import_stb/*.h" )  
add_library("import_stb" STATIC ${STB_SOURCES_FILES} )
source_group("sources"  FILES  ${STB_SOURCES_FILES})
set_target_properties("import_stb" PROPERTIES FOLDER "deps")

#######################################
# application_base
include_directories("${SOURCES_DIRECTORY}/application_base")
file(GLOB APPBASE_SOURCES_FILES  "${SOURCES_DIRECTORY}/application_base/*.cpp" "${SOURCES_DIRECTORY}/application_base/*.h" )  
add_library("application_base" STATIC ${APPBASE_SOURCES_FILES} )
source_group("sources"  FILES  ${APPBASE_SOURCES_FILES})
target_link_libraries("application_base" PUBLIC ${sdl_sdl_libs} import_imgui nfd import_stb d3d11.lib d3dcompiler.lib)

#######################################
# database
include_directories("${SOURCES_DIRECTORY}/database")
file(GLOB DB_SOURCES_FILES  "${SOURCES_DIRECTORY}/database/*.cpp" "${SOURCES_DIRECTORY}/database/*.h" )  
add_library("database" STATIC ${DB_SOURCES_FILES} )
source_group("sources"  FILES  ${DB_SOURCES_FILES})
target_precompile_headers("database" PRIVATE "${SOURCES_DIRECTORY}/database/database_header.h")
target_link_libraries("database" PUBLIC "sqlite3")
target_link_libraries("database" PUBLIC "application_base")
target_link_libraries("database" PUBLIC "SQLiteCpp")

#######################################
# actions
include_directories("${SOURCES_DIRECTORY}/actions")
file(GLOB ACTIONS_SOURCES_FILES  "${SOURCES_DIRECTORY}/actions/*.cpp" "${SOURCES_DIRECTORY}/actions/*.h" )  
add_library("actions" STATIC ${ACTIONS_SOURCES_FILES} )
source_group("sources"  FILES  ${ACTIONS_SOURCES_FILES})
target_precompile_headers("actions" PRIVATE "${SOURCES_DIRECTORY}/actions/actions_header.h")
target_link_libraries("actions" PUBLIC "application_base")
target_link_libraries("actions" PUBLIC "database")

#######################################
# ui
include_directories("${SOURCES_DIRECTORY}/ui")
file(GLOB UI_SOURCES_FILES  "${SOURCES_DIRECTORY}/ui/*.cpp" "${SOURCES_DIRECTORY}/ui/*.h" )  
add_library("ui" STATIC ${UI_SOURCES_FILES} )
source_group("sources"  FILES  ${UI_SOURCES_FILES})
target_precompile_headers("ui" PRIVATE "${SOURCES_DIRECTORY}/ui/ui_header.h")
target_link_libraries("ui" PUBLIC "application_base")
target_link_libraries("ui" PUBLIC "database")
target_link_libraries("ui" PUBLIC "actions")

#######################################
# application
include_directories("${SOURCES_DIRECTORY}/application")
file(GLOB EXE_FILES	"${SOURCES_DIRECTORY}/application/*.cpp" "${SOURCES_DIRECTORY}/application/*.h")
source_group("sources"  FILES  ${EXE_FILES})
add_executable(LImageDB ${EXE_FILES})

target_link_libraries(LImageDB PUBLIC application_base database actions ui )

set_target_properties(LImageDB PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
