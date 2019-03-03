set(headers
    vge_utility.h
)

set(source
    vge_utility.cpp
)

add_library(vge_utility
    ${headers}
    ${source}
)

target_include_directories(vge_utility PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(vge_utility
    vge_debug
    vge_core
)
