include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory(Server)
add_subdirectory(Parser)
add_subdirectory(Types)
add_subdirectory(Validators)
add_subdirectory(Resources)
set(polycube_server_sources ${polycube_server_sources} PARENT_SCOPE)