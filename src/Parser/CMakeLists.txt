find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)
bison_target(XPathParser XPath.yy ${CMAKE_CURRENT_LIST_DIR}/XPathParser.cpp VERBOSE)
flex_target(XPathScanner XPath.flex ${CMAKE_CURRENT_LIST_DIR}/XPathScanner.cpp COMPILE_FLAGS "-Cm -B -L --c++ --nounistd")
add_flex_bison_dependency(XPathScanner XPathParser)

add_compile_definitions(PARSER_INCLUDE="${BISON_XPathParser_OUTPUT_HEADER}")

set_source_files_properties(${BISON_XPathParser_OUTPUTS}
                            ${FLEX_XPathScanner_OUTPUTS}
                            PROPERTIES
                            COMPILE_FLAGS -Wno-sign-compare
                            COMPILE_FLAGS -Wno-effc++)

set(parser_sources ${CMAKE_CURRENT_LIST_DIR}/Yang.cpp ${CMAKE_CURRENT_LIST_DIR}/XPathParserDriver.cpp)

add_library(parser OBJECT
        ${BISON_XPathParser_OUTPUTS}
        ${FLEX_XPathScanner_OUTPUTS}
        ${parser_sources})

target_include_directories(parser PRIVATE ${PROJECT_SOURCE_DIR}/include/Parser)
