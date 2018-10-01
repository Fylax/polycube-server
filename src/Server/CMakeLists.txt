set(server_sources
        ${CMAKE_CURRENT_LIST_DIR}/Base64.cpp
        ${CMAKE_CURRENT_LIST_DIR}/RestServer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ResponseGenerator.cpp)

add_library(server OBJECT ${server_sources})

target_include_directories(server PRIVATE ${PROJECT_SOURCE_DIR}/include/Types)