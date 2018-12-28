set(headers
    vge_algorithm.h
)

set(source

)

add_library(vge_algorithm
    ${headers}
    ${source}
)

target_include_directories(vge_algorithm PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(vge_algorithm
    vge_core
)
