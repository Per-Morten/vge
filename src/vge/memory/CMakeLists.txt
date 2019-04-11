set(headers
    vge_allocator.h
    vge_memory.h
    vge_global_allocator.h
    vge_linear_allocator.h
    vge_memory_manager.h
)

set(source
    vge_allocator.cpp
    vge_malloc_allocator.cpp
    vge_global_allocator.cpp
    vge_linear_allocator.cpp
    vge_memory_manager.cpp
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
    vge_debug
    vge_third_party
    vge_container
)
