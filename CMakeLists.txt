cmake_minimum_required (VERSION 3.15)

set(TARGET_NAME supercow-mod)

project(${TARGET_NAME})

set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
include(FetchContent)

FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_SHALLOW	ON
    GIT_TAG yaml-cpp-0.7.0
)
FetchContent_GetProperties(yaml-cpp)

if(NOT yaml-cpp_POPULATED)
	message(STATUS "Populating yaml-cpp...")
	FetchContent_Populate(yaml-cpp)
	set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "disable yaml tests")
    set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "disable yaml tools")
    set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "disable yaml contrib")
    set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded)
	add_subdirectory(${yaml-cpp_SOURCE_DIR} ${yaml-cpp_BINARY_DIR})
	message(STATUS "Done.")
endif()
include_directories(BEFORE SYSTEM ${yaml-cpp_SOURCE_DIR} ${yaml-cpp_BINARY_DIR}/include)

FetchContent_Declare(
    libluajit
    GIT_REPOSITORY https://github.com/WohlSoft/LuaJIT.git
    GIT_SHALLOW	ON
    GIT_TAG 113eb1d172dfb6b91f67e9d0478a113a3ccced80
)
FetchContent_GetProperties(libluajit)

if(NOT libluajit_POPULATED)
    message(STATUS "Populating libluajit...")
    FetchContent_Populate(libluajit)
    set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded)
    add_subdirectory(${libluajit_SOURCE_DIR} ${libluajit_BINARY_DIR})
    message(STATUS "Done.")
endif()
include_directories(BEFORE SYSTEM ${libluajit_SOURCE_DIR}/src)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_GENERATOR_PLATFORM Win32)

if (NOT VERSION)
    set(VERSION manual_build)
endif()

message(STATUS "Building version " ${VERSION})

set(DEFS
    # Compliation
    -DCXX_COMPILER_ID="${CMAKE_CXX_COMPILER_ID}"
    -D_SILENCE_CXX20_CODECVT_FACETS_DEPRECATION_WARNING
    -D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

    # Platform
    -DUNICODE
    -D_UNICODE
    -DNOMINMAX
    -DVERSION="${VERSION}"
)

file(GLOB_RECURSE PROJECT_SOURCE_FILES "src/*.h" "src/*.cpp" "include/*.h" "src/proxy/dinput8.def")
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui)
set(IMGUI_DX8_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui-dx8)
set(IMGUI_CAPI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui-capi)
set(IMGUI_FILES
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/backends/imgui_impl_win32.cpp
    ${IMGUI_DX8_DIR}/imgui_impl_dx8.cpp
    ${IMGUI_CAPI_DIR}/cimgui.h
    ${IMGUI_CAPI_DIR}/cimgui.cpp
)

if(WIN32 AND (MSVC OR CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
    add_definitions(/MT /MP /FS /utf-8)
endif()

add_library(${TARGET_NAME} SHARED "${PROJECT_SOURCE_FILES}" "${IMGUI_FILES}")
set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME "dinput8")
# add_executable(${TARGET_NAME} "${PROJECT_SOURCE_FILES}")
target_compile_definitions(${TARGET_NAME} PRIVATE ${DEFS} )
include_directories(${TARGET_NAME} PRIVATE "src" "libs" "include" "libs/imgui")
target_link_libraries(${TARGET_NAME} ${PROJECT_SOURCE_DIR}/libs/minhook.lib libluajit yaml-cpp)