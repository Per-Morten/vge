set(headers
    vge_profiler.h
    vge_imgui_windows.h
)

set(source
    vge_profiler.cpp
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
)
