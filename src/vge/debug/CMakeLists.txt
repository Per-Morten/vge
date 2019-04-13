set(headers
    vge_assert.h
    vge_debug.h
    vge_imgui.h
    vge_log.h
    vge_profiler.h
)

set(source
    vge_imgui.cpp
    vge_log.cpp
    vge_profiler.cpp
    vge_debug.cpp
)

add_library(vge_debug
    ${headers}
    ${source}
)

target_include_directories(vge_debug PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(vge_debug
    vge_core
    vge_memory
    vge_third_party
    vge_gfx
)
