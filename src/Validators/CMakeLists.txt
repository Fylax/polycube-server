include_directories(${PROJECT_SOURCE_DIR}/include/Validators)

set(polycube_server_sources
   ${polycube_server_sources}
   ${CMAKE_CURRENT_LIST_DIR}/PatternValidator.cpp
   ${CMAKE_CURRENT_LIST_DIR}/Validators.cpp
   ${CMAKE_CURRENT_LIST_DIR}/EnumValidator.cpp
   ${CMAKE_CURRENT_LIST_DIR}/NumberValidators.cpp
   PARENT_SCOPE)