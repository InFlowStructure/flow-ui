cmake_minimum_required(VERSION 3.10)

if(NOT PROJECT_NAME)
  add_compile_definitions(FLOW_UI_EXPORT)
endif()

project(Flow VERSION 1.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
  string(TOUPPER ${CONFIG} CONFIG)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG} ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
endforeach(CONFIG CMAKE_CONFIGURATION_TYPES)

if(APPLE)
  enable_language(OBJC)
elseif(MSVC)
  add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

# -----------------------------------------------------------------------------
# Dependencies
# -----------------------------------------------------------------------------

if(flow-ui_INSTALL)
  set(flow-core_INSTALL ON CACHE BOOL "" FORCE)
  set(SPDLOG_INSTALL ON CACHE BOOL "" FORCE)
  set(NFD_INSTALL ON CACHE BOOL "" FORCE)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(cmake/CPM.cmake)

CPMAddPackage("gh:InFlowStructure/flow-core@1.2.0")
CPMAddPackage("gh:btzy/nativefiledialog-extended#29e3bcb")

FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/pthom/imgui.git
  GIT_TAG        imgui_bundle
)
FetchContent_Declare(
  imgui_node_editor
  GIT_REPOSITORY https://github.com/pthom/imgui-node-editor.git
  GIT_TAG        imgui_bundle
)
FetchContent_MakeAvailable(imgui imgui_node_editor)

set(HELLOIMGUI_DOWNLOAD_FREETYPE_IF_NEEDED OFF CACHE BOOL "")
set(HELLOIMGUI_USE_FREETYPE OFF CACHE BOOL "")
set(HELLOIMGUI_USE_EXTERNAL_JSON ON CACHE BOOL "")
set(HELLOIMGUI_BUILD_IMGUI OFF CACHE BOOL "" FORCE)
set(HELLOIMGUI_IMGUI_SOURCE_DIR ${imgui_SOURCE_DIR} CACHE STRING "" FORCE)

if (UNIX)
    add_compile_options(-fPIC)
endif()

CPMAddPackage("gh:pthom/hello_imgui#c985031")

include(FetchContent)
include(cmake/add_libs.cmake)
include(${hello_imgui_SOURCE_DIR}/hello_imgui_cmake/msvc/msvc_target_group.cmake)

add_imgui(${imgui_SOURCE_DIR})
add_simple_external_library_with_sources(imgui_node_editor ${imgui_node_editor_SOURCE_DIR})

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
CPMAddPackage("gh:gabime/spdlog@1.15.1")

# -----------------------------------------------------------------------------
# Library
# -----------------------------------------------------------------------------

if(MSVC)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
endif()

set(flow-ui_HEADERS_DIR "${CMAKE_CURRENT_LIST_DIR}/include/flow/ui")
file(GLOB flow-ui_HEADERS "${flow-ui_HEADERS_DIR}/*.hpp")
file(GLOB flow-ui_UTILITY_HEADERS "${flow-ui_HEADERS_DIR}/utilities/*.hpp")
file(GLOB flow-ui_VIEW_HEADERS "${flow-ui_HEADERS_DIR}/views/*.hpp")
file(GLOB flow-ui_WIDGET_HEADERS "${flow-ui_HEADERS_DIR}/widgets/*.hpp")
file(GLOB flow-ui_WINDOW_HEADERS "${flow-ui_HEADERS_DIR}/windows/*.hpp")
list(APPEND flow-ui_HEADERS
  ${flow-ui_UTILITY_HEADERS}
  ${flow-ui_VIEW_HEADERS}
  ${flow-ui_WIDGET_HEADERS}
  ${flow-ui_WINDOW_HEADERS}
)

add_library(flow-ui SHARED
  # Main source files
  src/Config.cpp
  src/Core.cpp
  src/Editor.cpp
  src/FileExplorer.cpp
  src/Style.cpp
  src/Texture.cpp
  src/ViewFactory.cpp
  src/Window.cpp

  # View source files
  src/views/ConnectionView.cpp
  src/views/PortView.cpp
  src/views/NodeView.cpp

  # Window source files
  src/windows/GraphWindow.cpp
  src/windows/ModuleManagerWindow.cpp
  src/windows/NewModuleWindow.cpp
  src/windows/NodeExplorerWindow.cpp
  src/windows/PropertyWindow.cpp
  src/windows/ShortcutsWindow.cpp

  # Widget source files
  src/widgets/InputField.cpp
  src/widgets/PropertyTree.cpp
  src/widgets/Table.cpp
  src/widgets/Text.cpp

  # Utility files
  src/utilities/Builders.cpp
  src/utilities/Widgets.cpp

  ${flow-ui_HEADERS}
)

add_library(flow-ui::flow-ui ALIAS flow-ui)

if(MSVC)
  add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
  target_compile_options(flow-ui PRIVATE /W4)
endif()

target_include_directories(flow-ui
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/flow/ui
    ${PROJECT_SOURCE_DIR}/include/flow/ui/views
    ${PROJECT_SOURCE_DIR}/include/flow/ui/windows
    ${PROJECT_SOURCE_DIR}/include/flow/ui/utilities
    ${PROJECT_SOURCE_DIR}/include/flow/ui/widgets
)

target_link_libraries(flow-ui PUBLIC
  flow-core::flow-core
  nlohmann_json::nlohmann_json
  spdlog::spdlog
)

target_link_libraries(flow-ui PRIVATE
  imgui
  imgui_node_editor
  hello_imgui
  nfd
)
target_compile_definitions(flow-ui PUBLIC IMGUI_DEFINE_MATH_OPERATORS)

add_subdirectory(programs/editor)

install(TARGETS flow-ui flow-core
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(DIRECTORY ${flow-core_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(DIRECTORY ${CPM_PACKAGE_json_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(DIRECTORY ${CPM_PACKAGE_thread-pool_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})


# -----------------------------------------------------------------------------
# Examples
# -----------------------------------------------------------------------------

option(BUILD_EXAMPLES OFF)
if (BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
