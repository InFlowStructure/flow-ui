if (NOT flow-ui_USE_EXTERNAL_FLOW_CORE)
    set(flow-core_USE_EXTERNAL_JSON ON CACHE BOOL "")
    add_subdirectory(flow-core)
endif()

set(HELLOIMGUI_DOWNLOAD_FREETYPE_IF_NEEDED OFF CACHE BOOL "")
set(HELLOIMGUI_USE_FREETYPE OFF CACHE BOOL "")

include(add_libs)
include(hello_imgui/hello_imgui_cmake/hello_imgui_add_app.cmake)
include(hello_imgui/hello_imgui_cmake/msvc/msvc_target_group.cmake)

add_hello_imgui()
add_simple_external_library_with_sources(imgui_node_editor ${CMAKE_CURRENT_SOURCE_DIR}/imgui-node-editor)

add_subdirectory(nfd)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
add_subdirectory(spdlog)
