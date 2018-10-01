cmake_minimum_required(VERSION 3.8)
project(service_controller LANGUAGES C CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
find_package(LibYANG REQUIRED)
find_package(Pistache CONFIG REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -Weffc++")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(src)

add_executable(service_controller main.cpp
        $<TARGET_OBJECTS:parser>
        $<TARGET_OBJECTS:resources>
        $<TARGET_OBJECTS:server>
        $<TARGET_OBJECTS:types>
        $<TARGET_OBJECTS:validators>)

target_link_libraries(service_controller yang pistache)
