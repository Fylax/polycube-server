include_directories(${PROJECT_SOURCE_DIR}/include/Resources)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/externals/include/nlohmann)

set(polycube_server_sources
        ${polycube_server_sources}
        ${CMAKE_CURRENT_LIST_DIR}/CubeManager.cpp
        ${CMAKE_CURRENT_LIST_DIR}/Cube.cpp
        ${CMAKE_CURRENT_LIST_DIR}/Resource.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ParentResource.cpp
        ${CMAKE_CURRENT_LIST_DIR}/LeafResource.cpp
        ${CMAKE_CURRENT_LIST_DIR}/PathParamField.cpp
        ${CMAKE_CURRENT_LIST_DIR}/JsonBodyField.cpp
        PARENT_SCOPE)