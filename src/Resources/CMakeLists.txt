add_subdirectory(Body)
add_subdirectory(Endpoint)
add_subdirectory(Data)
set(resources_sources
        ${CMAKE_CURRENT_LIST_DIR}/ServiceManager.cpp
        ${body_sources}
        ${endpoint_sources}
        ${data_sources})

add_library(resources OBJECT ${resources_sources})

target_include_directories(resources PRIVATE ${body_include} ${endpoint_include} ${data_include})
target_include_directories(resources SYSTEM INTERFACE ${PROJECT_SOURCE_DIR}/externals/include/nlohmann)