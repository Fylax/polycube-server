include_directories(${PROJECT_SOURCE_DIR}/include/Parser)

set(polycube_server_sources
        ${polycube_server_sources}
        ${CMAKE_CURRENT_LIST_DIR}/Parser.cpp
        PARENT_SCOPE)