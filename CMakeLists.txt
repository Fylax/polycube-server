cmake_minimum_required(VERSION 3.9.4)
project(service_controller LANGUAGES C CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
find_package(LibYANG REQUIRED)
find_package(Pistache CONFIG REQUIRED)

add_compile_options(-Wall -pedantic -Wextra -Weffc++)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_BUILD_TYPE MATCHES "[Re][Ee][Ll][Ee][Aa][Ss][Ee]")
  include(CheckIPOSupported)
  check_ipo_supported(RESULT supported OUTPUT error)
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

add_subdirectory(src)

add_executable(service_controller main.cpp
        $<TARGET_OBJECTS:parser>
        $<TARGET_OBJECTS:resources>
        $<TARGET_OBJECTS:server>
        $<TARGET_OBJECTS:types>
        $<TARGET_OBJECTS:validators>)

target_link_libraries(service_controller yang pistache ${CMAKE_DL_LIBS})

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION FALSE)
add_subdirectory(test-service)
