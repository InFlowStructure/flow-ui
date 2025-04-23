cmake_minimum_required(VERSION 3.10)

project(simple_module VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(APPLE)
    enable_language(OBJC)
endif()

add_library(${PROJECT_NAME} SHARED src/register.cpp)
target_compile_definitions(${PROJECT_NAME} PRIVATE FLOW_SIMPLE_EXPORT)

if(MSVC)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4)
endif()

add_dependencies(${PROJECT_NAME} flow-core)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PUBLIC flow-core)
