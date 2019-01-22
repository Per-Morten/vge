set(headers
    vge_shader.h
    vge_texture.h
    vge_gfx_manager.h
    vge_gfx.h
    vge_gl_error.h
)

set(source
    vge_shader.cpp
    vge_texture.cpp
    vge_gfx_manager.cpp
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
)
