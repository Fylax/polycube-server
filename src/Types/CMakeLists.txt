set(types_sources ${CMAKE_CURRENT_LIST_DIR}/Decimal64.cpp)

add_library(types OBJECT ${types_sources})

target_include_directories(types PRIVATE ${PROJECT_SOURCE_DIR}/include/Types)
