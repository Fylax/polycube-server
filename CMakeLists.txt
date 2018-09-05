cmake_minimum_required(VERSION 3.8)
project(parser LANGUAGES C CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
find_package(LibYANG REQUIRED)
find_package(Pistache CONFIG REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")

if ("cxx_std_17" IN_LIST CMAKE_CXX_COMPILE_FEATURES)
    set(CMAKE_CXX_STANDARD 17)
else ()
    set(CMAKE_CXX_STANDARD 11)
endif ()

set(polycube_server_sources main.cpp)
add_subdirectory(src)

add_executable(parser
        ${polycube_server_sources})
target_link_libraries(parser yang pistache)

target_compile_features(parser PRIVATE cxx_std_17)
