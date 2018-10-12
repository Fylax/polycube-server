add_subdirectory(Endpoint)
add_subdirectory(Body)
set(resources_sources
        ${CMAKE_CURRENT_LIST_DIR}/CubeManager.cpp
        ${body_sources}
        ${endpoint_sources})

add_library(resources OBJECT ${resources_sources})

target_include_directories(resources PRIVATE ${PROJECT_SOURCE_DIR}/include/Resources ${body_include} ${endpoint_include})
target_include_directories(resources SYSTEM INTERFACE ${PROJECT_SOURCE_DIR}/externals/include/nlohmann)