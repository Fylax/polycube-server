include_directories(${PROJECT_SOURCE_DIR}/include/Types)

set(polycube_server_sources
        ${polycube_server_sources}
        ${CMAKE_CURRENT_LIST_DIR}/Base64.cpp
        ${CMAKE_CURRENT_LIST_DIR}/RestServer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ResponseGenerator.cpp
        PARENT_SCOPE)