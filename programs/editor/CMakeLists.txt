cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -----------------------------------------------------------------------------
# Dependencies
# -----------------------------------------------------------------------------

CPMAddPackage("gh:jarro2783/cxxopts@3.2.0")

# -----------------------------------------------------------------------------
# Executable
# -----------------------------------------------------------------------------

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
endif()

if(PACK)
   set(INSTALL_DIR bin)
   set(LIB_DIR bin/lib)
else(PACK)
   set(INSTALL_DIR ${PROJECT_BINARY_DIR}/bin)
   set(LIB_DIR ${PROJECT_BINARY_DIR}/lib)
endif(PACK)

hello_imgui_add_app(FlowEditor
   src/main.cpp
)
target_link_libraries(FlowEditor PUBLIC
  flow-ui::flow-ui
  cxxopts
)

add_custom_command(TARGET FlowEditor POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/templates
    $<TARGET_FILE_DIR:FlowEditor>/templates
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/templates DESTINATION ${CMAKE_INSTALL_PREFIX})
