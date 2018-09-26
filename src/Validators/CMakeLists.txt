include_directories(${PROJECT_SOURCE_DIR}/include/Validators)

set(polycube_server_sources
        ${polycube_server_sources}
        ${CMAKE_CURRENT_LIST_DIR}/BitsValidator.cpp
        ${CMAKE_CURRENT_LIST_DIR}/BoolValidator.cpp
        ${CMAKE_CURRENT_LIST_DIR}/EmptyValidator.cpp
        ${CMAKE_CURRENT_LIST_DIR}/EnumValidator.cpp
        ${CMAKE_CURRENT_LIST_DIR}/InSetValidator.cpp
        ${CMAKE_CURRENT_LIST_DIR}/LengthValidator.cpp
        ${CMAKE_CURRENT_LIST_DIR}/NumberValidators.cpp
        ${CMAKE_CURRENT_LIST_DIR}/PatternValidator.cpp
        ${CMAKE_CURRENT_LIST_DIR}/UnionValidator.cpp
        ${CMAKE_CURRENT_LIST_DIR}/XPathValidator.cpp
        PARENT_SCOPE)