set(headers
    vge_container.h
    vge_string.h
    vge_array.h
    vge_slot_map.h
)

set(source
    vge_array.tpp
    vge_slot_map.tpp
)

add_library(vge_container
    ${headers}
    ${source}
)

target_include_directories(vge_container PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(vge_container
    vge_core
    vge_memory
    vge_debug
)
