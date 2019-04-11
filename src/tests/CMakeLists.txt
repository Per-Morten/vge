set(test_files
    test_vge_slot_map.cpp
    test_vge_thread.cpp
    test_vge_allocator.cpp
)

add_executable(vge_tests vge_tests.cpp ${test_files})

target_link_libraries(vge_tests
    vge_core
    vge_algorithm
    vge_debug
    vge_container
)
