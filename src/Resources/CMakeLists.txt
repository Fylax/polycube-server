include_directories(${PROJECT_SOURCE_DIR}/include/Reources)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/externals/include/nlohmann)

set(polycube_server_sources
        ${polycube_server_sources}
        ${CMAKE_CURRENT_LIST_DIR}/Resource.cpp
        ${CMAKE_CURRENT_LIST_DIR}/Field.cpp
        ${CMAKE_CURRENT_LIST_DIR}/PathParamField.cpp
        ${CMAKE_CURRENT_LIST_DIR}/JsonBodyField.cpp
        PARENT_SCOPE)