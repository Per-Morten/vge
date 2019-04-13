set(headers
    vge_gfx_manager.h
    vge_gfx.h
    vge_gfx_gl.h
    vge_obj_loader.h
    vge_color.h
    vge_color.cpp
)

set(source
    vge_gfx_manager.cpp
    vge_gfx_gl.cpp
    vge_obj_loader.cpp
)

add_library(vge_gfx
    ${headers}
    ${source}
)

target_include_directories(vge_gfx PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(vge_gfx
    vge_debug
    vge_core
    vge_third_party
    vge_utility
    vge_container
)
