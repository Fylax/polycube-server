set(body_sources
        ${CMAKE_CURRENT_LIST_DIR}/AbstractFactory.cpp
        ${CMAKE_CURRENT_LIST_DIR}/Resource.cpp
        ${CMAKE_CURRENT_LIST_DIR}/LeafResource.cpp
        ${CMAKE_CURRENT_LIST_DIR}/LeafListResource.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ParentResource.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ListResource.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ChoiceResource.cpp
        ${CMAKE_CURRENT_LIST_DIR}/CaseResource.cpp
        ${CMAKE_CURRENT_LIST_DIR}/Service.cpp
        ${CMAKE_CURRENT_LIST_DIR}/JsonBodyField.cpp
        PARENT_SCOPE)

set(body_include ${PROJECT_SOURCE_DIR}/include/Resources/Endpoint PARENT_SCOPE)