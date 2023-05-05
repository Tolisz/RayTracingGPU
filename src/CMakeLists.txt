add_executable(rt_gpu 
    main.cpp
    camera.cpp
    spheres_world.cpp
    materials.cpp
    aabb.cpp
    random.cpp
    BVH_tree.cpp
    textures.cpp
)

target_link_libraries(rt_gpu OpenCL)

set(EXECUTABLE_OUTPUT_PATH ${ray_tracing_gpu_SOURCE_DIR})