cmake_minimum_required(VERSION 3.29)
project(School_Project1 C)
set(CMAKE_C_STANDARD 99)
add_executable(School_Project1 main.c)

set(CURL_INCLUDE_DIR "C:/vcpkg/vcpkg/installed/x64-windows/include")
set(CURL_LIBRARY "C:/vcpkg/vcpkg/installed/x64-windows/lib/libcurl.lib")
include_directories(${CURL_INCLUDE_DIR})
target_link_libraries(School_Project1 ${CURL_LIBRARY})

set(RAYLIB_INCLUDE_DIR "C:/vcpkg/vcpkg/installed/x64-windows/include")
set(RAYLIB_LIBRARY "C:/vcpkg/vcpkg/installed/x64-windows/lib/raylib.lib")
include_directories(${RAYLIB_INCLUDE_DIR})
target_link_libraries(School_Project1 ${RAYLIB_LIBRARY})