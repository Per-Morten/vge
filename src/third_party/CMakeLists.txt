set(headers
    # IMGUI
    imgui/imgui.h
    imgui/examples/imgui_impl_glfw.h

    # STB
    stb/stb_image.h

    # GLAD
    glad/glad.h

    # TinyObj
    tiny_obj_loader/tiny_obj_loader.h

    # Catch
    catch/catch.h
)

set(source
    # IMGUI
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_widgets.cpp
    imgui/examples/imgui_impl_glfw.cpp
    imgui/examples/imgui_impl_opengl3.cpp

    # STB
    stb/stb_image.cpp

    # GLAD
    glad/glad.c

    # TinyObj
    tiny_obj_loader/tiny_obj_loader.cpp
)

add_library(vge_third_party
    ${headers}
    ${source}
)

target_include_directories(vge_third_party PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui/examples"
    "${CMAKE_CURRENT_SOURCE_DIR}/stb"
    "${CMAKE_CURRENT_SOURCE_DIR}/tiny_obj_loader"
    "${CMAKE_CURRENT_SOURCE_DIR}/catch"
)

add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLAD)

target_link_libraries(vge_third_party
    dl
)

