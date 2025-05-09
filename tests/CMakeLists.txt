add_executable(tests "tests.cpp")
target_link_libraries(tests strassen3 benchmark::benchmark_main)
