add_subdirectory(Body)

set(lib_sources
        ${body_source}
        PARENT_SCOPE)

set(lib_include ${body_include} PARENT_SCOPE)