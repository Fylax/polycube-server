set(resources_sources
        ${CMAKE_CURRENT_LIST_DIR}/CubeManager.cpp
        ${CMAKE_CURRENT_LIST_DIR}/Cube.cpp
        ${CMAKE_CURRENT_LIST_DIR}/Resource.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ParentResource.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ListResource.cpp
        ${CMAKE_CURRENT_LIST_DIR}/LeafResource.cpp
        ${CMAKE_CURRENT_LIST_DIR}/PathParamField.cpp
        ${CMAKE_CURRENT_LIST_DIR}/JsonBodyField.cpp)

add_library(resources OBJECT ${resources_sources})

target_include_directories(resources PRIVATE ${PROJECT_SOURCE_DIR}/include/Resources)
target_include_directories(resources SYSTEM INTERFACE ${PROJECT_SOURCE_DIR}/externals/include/nlohmann)