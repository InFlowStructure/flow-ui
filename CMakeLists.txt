cmake_minimum_required(VERSION 3.10)

if(NOT PROJECT_NAME)
  add_compile_definitions(FLOW_UI_EXPORT)
endif()

project(flow-ui VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(APPLE)
  enable_language(OBJC)
endif()

# -----------------------------------------------------------------------------
# Options
# -----------------------------------------------------------------------------

option(flow-ui_USE_EXTERNAL_FLOW_CORE "Use an external Flow Core library" OFF)

# -----------------------------------------------------------------------------
# Dependencies
# -----------------------------------------------------------------------------

find_package(nlohmann_json CONFIG QUIET)
if (NOT nlohmann_json_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    nlohmann_json OVERRIDE_FIND_PACKAGE
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
  )
  FetchContent_MakeAvailable(nlohmann_json)
endif()

if (flow-ui_USE_EXTERNAL_FLOW_CORE)
  find_package(flow-core REQUIRED)
endif()

if(flow-ui_INSTALL)
  set(IMGUI_BUNDLE_INSTALL_CPP ON CACHE BOOL "" FORCE)
  set(SPDLOG_INSTALL ON CACHE BOOL "" FORCE)
endif()

add_subdirectory(third_party)

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
file(GLOB flow-ui_WINDOW_HEADERS "${flow-ui_HEADERS_DIR}/windows/*.hpp")
list(APPEND flow-ui_HEADERS
  ${flow-ui_UTILITY_HEADERS}
  ${flow-ui_VIEW_HEADERS}
  ${flow-ui_WINDOW_HEADERS}
)

add_library(${PROJECT_NAME} SHARED
  # Main source files
  src/Config.cpp
  src/Style.cpp
  src/Texture.cpp
  src/ViewFactory.cpp
  src/Window.cpp
  src/FileStorage.cpp
  src/Editor.cpp

  # View source files
  src/views/LinkView.cpp
  src/views/NodeView.cpp
  src/views/PortView.cpp

  # Window source files
  src/windows/GraphWindow.cpp
  src/windows/ImageInspectorWindow.cpp
  src/windows/LogWindow.cpp
  src/windows/PropertyWindow.cpp

  # Utility files
  src/utilities/Builders.cpp
  src/utilities/Widgets.cpp

  ${flow-ui_HEADERS}
)

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

if(MSVC)
  add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4)
endif()

target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/flow/ui
    ${PROJECT_SOURCE_DIR}/include/flow/ui/views
    ${PROJECT_SOURCE_DIR}/include/flow/ui/windows
    ${PROJECT_SOURCE_DIR}/include/flow/ui/utilities
    ${PROJECT_SOURCE_DIR}/include/flow/ui/widgets
)

target_link_libraries(${PROJECT_NAME}
  flow-core::flow-core
  nlohmann_json::nlohmann_json
  spdlog::spdlog
  imgui_bundle
  nfd
)
target_compile_definitions(${PROJECT_NAME} PUBLIC IMGUI_DEFINE_MATH_OPERATORS)

if(MSVC)
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
  )
endif()

# -----------------------------------------------------------------------------
# Install
# -----------------------------------------------------------------------------

# TODO: Figure out how to do this nicely with these dependencies.
# if(flow-ui_INSTALL)
#   set(export_destination "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")
#   set(export_targets
#     ${PROJECT_NAME}
#     ${IMGUI_BUNDLE_INSTALLABLE_DEPENDENCIES}
#     spdlog
#     nfd
#   )

#   if (NOT flow-ui_USE_EXTERNAL_FLOW_CORE)
#     list(APPEND export_targets flow-core)
#     list(APPEND export_targets nlohmann_json)
#   endif()

#   install(DIRECTORY include/ DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
#   install(TARGETS ${export_targets}
#     EXPORT ${PROJECT_NAME}
#     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
#   )

#   export(TARGETS ${export_targets}
#     NAMESPACE ${PROJECT_NAME}::
#     FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake"
#   )
#   install(EXPORT ${PROJECT_NAME} DESTINATION ${export_destination} NAMESPACE ${PROJECT_NAME}:: FILE "${PROJECT_NAME}Targets.cmake")

#   include(CMakePackageConfigHelpers)
#   configure_package_config_file(
#     "${CMAKE_CURRENT_LIST_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
#     "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
#     INSTALL_DESTINATION ${export_destination}
#   )

#   install(
#     FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
#     DESTINATION ${export_destination}
#   )

#   write_basic_package_version_file(${PROJECT_NAME}ConfigVersion.cmake COMPATIBILITY SameMajorVersion)
# endif()
