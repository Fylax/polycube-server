set(parser_sources ${CMAKE_CURRENT_LIST_DIR}/Parser.cpp)

add_library(parser OBJECT ${parser_sources})

target_include_directories(parser PRIVATE ${PROJECT_SOURCE_DIR}/include/Parser)
