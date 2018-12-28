set(headers
    vge_allocator.h
    vge_memory.h
)

set(source
    vge_allocator.cpp
    vge_malloc_allocator.cpp
)

add_library(vge_memory
    ${headers}
    ${source}
)

target_include_directories(vge_memory PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(vge_memory
    vge_core
    vge_algorithm
)
