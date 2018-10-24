set(body_sources
        ${CMAKE_CURRENT_LIST_DIR}/ConcreteFactory.cpp
        ${CMAKE_CURRENT_LIST_DIR}/EntryPoint.cpp
        ${CMAKE_CURRENT_LIST_DIR}/LeafResource.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ParentResource.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ListResource.cpp
        ${CMAKE_CURRENT_LIST_DIR}/Service.cpp
        PARENT_SCOPE)

set(body_include ${PROJECT_SOURCE_DIR}/include/Resources/Data/Lib/Body PARENT_SCOPE)