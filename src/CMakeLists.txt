add_executable(rtgpu 
    main.cpp
)

target_link_libraries(rtgpu OpenCL)

set(EXECUTABLE_OUTPUT_PATH ${ray_tracing_gpu_SOURCE_DIR})