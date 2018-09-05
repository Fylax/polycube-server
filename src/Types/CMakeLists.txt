include_directories(${PROJECT_SOURCE_DIR}/include/Types)

set(polycube_server_sources
        ${polycube_server_sources}
        ${CMAKE_CURRENT_LIST_DIR}/Decimal64.cpp
        PARENT_SCOPE)