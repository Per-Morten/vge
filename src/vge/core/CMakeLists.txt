set(headers
    vge_assert.h
    vge_attributes.h
    vge_core.h
    vge_log.h
)

set(source
    vge_log.cpp
)

add_library(vge_core
    ${headers}
    ${source}
)

target_include_directories(vge_core PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

if (WIN32)

else()
    target_link_libraries(vge_core
        m #math
    )
endif()
