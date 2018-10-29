add_subdirectory(Lib)

set(data_sources
        ${CMAKE_CURRENT_LIST_DIR}/AbstractFactory.cpp
        ${lib_sources} PARENT_SCOPE)
set(data_include
        ${PROJECT_SOURCE_DIR}/include/Resources/Data
        ${lib_include} PARENT_SCOPE)