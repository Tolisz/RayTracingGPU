add_executable(rt_gpu 
    main.cpp
    camera.cpp
    clwrapper.cpp
    spheres.cpp
    materials.cpp
)

target_link_libraries(rt_gpu OpenCL)

set(EXECUTABLE_OUTPUT_PATH ${ray_tracing_gpu_SOURCE_DIR})