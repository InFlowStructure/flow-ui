function(add_imgui imgui_dir)
    if(NOT TARGET imgui)
        file(GLOB imgui_sources ${imgui_dir}/*.h ${imgui_dir}/*.cpp ${imgui_dir}/misc/cpp/*.cpp ${imgui_dir}/misc/cpp/*.h)
        add_library(imgui STATIC ${imgui_sources})

        set(imgui_SOURCE_DIR ${imgui_dir})
        target_include_directories(imgui PUBLIC
            $<BUILD_INTERFACE:${imgui_SOURCE_DIR}>
            $<BUILD_INTERFACE:${imgui_SOURCE_DIR}/backends>
            $<BUILD_INTERFACE:${imgui_SOURCE_DIR}/misc/cpp>
            $<BUILD_INTERFACE:${imgui_SOURCE_DIR}/misc/freetype>
        )

        if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
            target_compile_options(imgui PRIVATE -Wno-exceptions)
        elseif (CMAKE_COMPILER_IS_GNUCC)
            target_compile_options(imgui PRIVATE -Wno-terminate)
        elseif(MSVC)
            target_compile_options(imgui PRIVATE "/wd4297")
        endif()

        if(PROJECT_IS_TOP_LEVEL AND NOT SKBUILD)
            install(TARGETS imgui DESTINATION ./lib/)
        endif()

        if(IMGUI_BUNDLE_BUILD_PYTHON)
            target_compile_definitions(imgui PUBLIC ImTextureID=int)
        endif()

        if (UNIX)
            target_compile_options(imgui PUBLIC -fPIC)
        endif()
        hello_imgui_msvc_target_group_sources(imgui)
    endif()
endfunction()

function (add_hello_imgui)
    if (UNIX)
        add_compile_options(-fPIC)
    endif()

    set(BUILD_SHARED_LIBS OFF)
    set(imgui_dir ${CMAKE_CURRENT_LIST_DIR}/imgui)
    add_imgui(${imgui_dir})

    set(HELLOIMGUI_BUILD_IMGUI OFF CACHE BOOL "" FORCE)
    set(HELLOIMGUI_IMGUI_SOURCE_DIR ${imgui_dir} CACHE STRING "" FORCE)

    add_subdirectory(hello_imgui)

    if (WIN32)
        set_target_properties(hello_imgui PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
    endif()
endfunction()


function(add_simple_external_library_with_sources lib_target_name lib_folder)
    file(GLOB lib_sources ${lib_folder}/*.cpp ${lib_folder}/*.h)

    add_library(${lib_target_name} STATIC ${lib_sources})
    target_link_libraries(${lib_target_name} PUBLIC imgui)

    set(lib_parent_folder ${lib_folder})
    target_include_directories(${lib_target_name} PUBLIC $<BUILD_INTERFACE:${lib_parent_folder}>)

    hello_imgui_msvc_target_group_sources(${lib_target_name})
endfunction()