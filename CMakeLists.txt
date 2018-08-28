cmake_minimum_required(VERSION 3.12)
project(parser LANGUAGES C CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
find_package(LibYANG REQUIRED)
find_package(Pistache CONFIG  REQUIRED)

set(CMAKE_CXX_STANDARD 14)

set(polycube_server_sources main.cpp)
add_subdirectory(src)


add_executable(parser
        ${polycube_server_sources})
target_link_libraries(parser yang pistache)
