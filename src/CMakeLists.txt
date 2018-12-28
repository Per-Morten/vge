cmake_minimum_required(VERSION 3.3.2)
project(vge)

string(TOLOWER ${CMAKE_BUILD_TYPE} VGE_OUTPUT_FOLDER)
set(CMAKE_BINARY_DIR "$ENV{VGE_BUILD_DIR}/engine/${VGE_OUTPUT_FOLDER}")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)


# Compiler setup
if (NOT WIN32)
    set(CMAKE_CXX_COMPILER clang++-6.0)
endif()

set(VGE_STANDARD_FLAGS "")
set(VGE_DEBUG_FLAGS "")
set(VGE_RELEASE_FALGS "")


set(VGE_STANDARD_FLAGS "${VGE_STANDARD_FLAGS}   \
                        -std=c++17              \
                        -Wall                   \
                        -Wextra                 \
                       "
)

set(VGE_DEBUG_FLAGS    "${VGE_DEBUG_FLAGS}      \
                        -g3                     \
                        -ggdb                   \
                        -O0                     \
                        -DVGE_ASSERT_TERMINATE  \
                       "
)

set(YM_RELEASE_FLAGS  "${VGE_RELEASE_FLAGS}     \
                        -O3                     \
                        -DVGE_ASSERT_REPORT     \
                      "
)

set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS}         \
     ${VGE_STANDARD_FLAGS}      \
    "
)

set(CMAKE_CXX_FLAGS_DEBUG
    "${CMAKE_CXX_FLAGS}         \
     ${VGE_DEBUG_FLAGS}         \
    "
)

set(CMAKE_CXX_FLAGS_RELEASE
    "${CMAKE_CXX_FLAGS_RELEASE}     \
     ${VGE_RELEASE_FLAGS}           \
    "
)

subdirs(
    vge
    third_party
)

if (${VGE_ENABLE_TESTS})
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS}         \
        -UVGE_ASSERT_TERMINATE      \
        -DVGE_ASSERT_ERRC           \
        "
    )
endif()

if (NOT ${CMAKE_BUILD_TYPE} STREQUAL "Test")

    add_executable(main main.cpp)

    target_link_libraries(main
        # External
        X11
        GL
        glfw
        GLEW

        # Internal
        vge_core
        vge_memory
        vge_algorithm
        vge_third_party

    )

    set_target_properties(main PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$ENV{VGE_ROOT_DIR}")
endif()