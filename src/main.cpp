#define CL_TARGET_OPENCL_VERSION 300

#ifdef MAC
#include <OpenCL/cl.h>
#else  
#include <CL/cl.h>
#endif

#include <iostream>
#include <cmath>
#include <cstdlib>

#include "error.h"
#include "utility_functions.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define SAMPLES_PER_PIXEL 100
#define MAX_RECURSION_DEPTH 10

#include "CL_wrap.hpp"

#include "camera.hpp"
#include "vec/vec.hpp"
#include "materials.hpp"
#include "spheres_world.hpp"
#include "random.hpp"
#include "BVH_tree.hpp"
#include "textures.hpp"

World random_scene() {

    World world;

    auto ground_material = std::make_shared<Lambertian>(vec::vec3(0.5, 0.5, 0.5));
    world.add(std::make_shared<Sphere>(vec::vec3(0,-1000,0), 1000, ground_material));

    auto random_vec3 = []() -> vec::vec3 
    {
        return vec::vec3(random_float(), random_float(), random_float()); 
    }; 

    auto random_vec3_mm = [](float min, float max) -> vec::vec3 
    {
        return vec::vec3(random_float(min, max), random_float(min, max), random_float(min, max)); 
    }; 

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_float();
            vec::vec3 center(a + 0.9*random_float(), 0.2, b + 0.9*random_float());

            if ((center - vec::vec3(4, 0.2, 0)).length() > 0.9) {
                std::shared_ptr<Material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = random_vec3() * random_vec3();
                    sphere_material = std::make_shared<Lambertian>(albedo);
                    auto center2 = center + vec::vec3(0, random_float(0.0f,0.5f), 0);

                    world.add(std::make_shared<Moving_Sphere>(center, center2, 0.0, 1.0, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = random_vec3_mm(0.5, 1);
                    auto fuzz = random_float(0, 0.5);
                    sphere_material = std::make_shared<Metal>(albedo, fuzz);
                    world.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = std::make_shared<Dielectric>(1.5);
                    world.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = std::make_shared<Dielectric>(1.5);
    world.add(std::make_shared<Sphere>(vec::vec3(0, 1, 0), 1.0, material1));

    auto material2 = std::make_shared<Lambertian>(vec::vec3(0.4, 0.2, 0.1));
    world.add(std::make_shared<Sphere>(vec::vec3(-4, 1, 0), 1.0, material2));

    auto material3 = std::make_shared<Metal>(vec::vec3(0.7, 0.6, 0.5), 0.0);
    world.add(std::make_shared<Sphere>(vec::vec3(4, 1, 0), 1.0, material3));

    return world;
}

World test_scene() 
{
    World world;

    auto ground_material = std::make_shared<Lambertian>(vec::vec3(0.5, 0.5, 0.5));
    world.add(std::make_shared<Sphere>(vec::vec3(0,-1000,0), 1000, ground_material));

    auto material1 = std::make_shared<Dielectric>(1.5);
    world.add(std::make_shared<Sphere>(vec::vec3(0, 1, 0), 1.0, material1));

    auto material2 = std::make_shared<Lambertian>(vec::vec3(0.4, 0.2, 0.1));
    world.add(std::make_shared<Sphere>(vec::vec3(-4, 1, 0), 1.0, material2));

    auto material3 = std::make_shared<Metal>(vec::vec3(0.7, 0.6, 0.5), 0.0);
    world.add(std::make_shared<Sphere>(vec::vec3(4, 1, 0), 1.0, material3));

    return world;
}

//  --------------------------------  //
//                MAIN                //
//  --------------------------------  // 

int main(int argc, char** argv)
{

    // -----------------------------
    //   WIRTUALNA SCENA (POCZĄTEK)
    // -----------------------------

    World world = test_scene();
    //World world = random_scene();

    std::cout << "Utworzylem scene" << std::endl;

    // Camera 

    vec::vec3 lookfrom(13,2,3);
    vec::vec3 lookat(0,0,0);
    vec::vec3 vup(0,1,0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.1;

    // Camera class parametrs
    float aspect_ratio = 9.0f / 16.0f;

    size_t image_width = 400;
    size_t image_height = image_width * aspect_ratio;

    // -----------------------------
    //   WIRTUALNA SCENA (KONIEC)
    // -----------------------------

    // Spheres 

    void* ptr_spheres;
    size_t ptr_spheres_size;
    size_t ptr_spheres_table_size;
    Sphere_List::get_cl_structure(&ptr_spheres, &ptr_spheres_size, &ptr_spheres_table_size);

    void* ptr_moving_spheres;
    size_t ptr_moving_spheres_size;
    size_t ptr_moving_spheres_table_size;
    Moving_Sphere_List::get_cl_structure(&ptr_moving_spheres, &ptr_moving_spheres_size, &ptr_moving_spheres_table_size);

    //Materials

    void* ptr_albedo = nullptr;
    size_t ptr_albedo_size;
    size_t ptr_albedo_table_size;
    Lambertian_List::get_cl_structure(&ptr_albedo, &ptr_albedo_size, &ptr_albedo_table_size);

    void* ptr_metal = nullptr;
    size_t ptr_metal_size;
    size_t ptr_metal_table_size;
    Metal_List::get_cl_structure(&ptr_metal, &ptr_metal_size, &ptr_metal_table_size);

    void* ptr_fuzz = nullptr;
    size_t ptr_fuzz_size;
    size_t ptr_fuzz_table_size;
    Dielectric_List::get_cl_structure(&ptr_fuzz, &ptr_fuzz_size, &ptr_fuzz_table_size);


    std::vector<std::shared_ptr<Object>> test_vector(world.objects.begin(), world.objects.end());
    std::cout << "Robie drzewo" << std::endl;
    BVH_tree tree(test_vector, 0.0f, 1.0f);

    void* ptr_BVH = nullptr;
    size_t ptr_BVH_size;
    size_t ptr_BVH_table_size;

    tree.get_cl_structure(&ptr_BVH, &ptr_BVH_size, &ptr_BVH_table_size);
    
    void* ptr_texture_solid = nullptr;
    size_t ptr_texture_solid_size;
    size_t ptr_texture_solid_table_size;
    Solid_Color_List::get_cl_structure(&ptr_texture_solid, &ptr_texture_solid_size, &ptr_texture_solid_table_size);

    cl_int err;

    // /* OpenCL platform */

    CL::CL_wrap cl_object;

    std::cout << "Stworzylem konieczne elementy OpenCL do wykorzystania w programie" << std::endl;

    cl_object.create_program("src_opencl/kelner.cl");

    /* OpenCL program and kelner*/

    //std::string kelner_name = "ray_tracer";

    cl_object.add_include_dir("./src_opencl");
    
    cl_object.add_define("NUMBER_OF_SPHERES"                , std::to_string(ptr_spheres_table_size));
    cl_object.add_define("NUM_OF_ALBEDO_MATERIALS"          , std::to_string(ptr_albedo_table_size));
    cl_object.add_define("NUM_OF_FUZZ_MATERIALS"            , std::to_string(ptr_fuzz_table_size));
    cl_object.add_define("NUM_OF_REFLECTANCE_MATERIALS"     , std::to_string(ptr_metal_table_size));
    cl_object.add_define("NUM_OF_MOVING_SPHERE"             , std::to_string(ptr_moving_spheres_table_size));
    cl_object.add_define("SAMPLES_PER_PIXEL"                , std::to_string(SAMPLES_PER_PIXEL));
    cl_object.add_define("MAX_RECURSION_DEPTH"              , std::to_string(MAX_RECURSION_DEPTH));
    cl_object.add_define("NUM_OF_BVH"                       , std::to_string(ptr_BVH_table_size));
    cl_object.add_define("BVH_HELP_TABLE_SIZE"              , std::to_string(ptr_BVH_table_size == 0 ? 1 : (int)std::ceil(std::log2f(ptr_BVH_table_size)) + 1));    
    cl_object.add_define("NUM_OF_TEX_SOLID_COLOR"           , std::to_string(ptr_texture_solid_table_size));

    std::cout << "BUILD OPTIONS = " << cl_object.get_build_options() << "\n";   

    cl_object.build_program();

    std::cout << "Zbudowalem program" << std::endl;

    cl_object.create_kernel("ray_tracer");

    std::cout << "Stworzylem kelner" << std::endl;

    /* Image parametrs and OpenCL image object creation */

    cl_image_format image_format;
    image_format.image_channel_order = CL_RGBA;
    image_format.image_channel_data_type = CL_UNSIGNED_INT8;

    cl_image_desc image_desc;
    image_desc.image_type = CL_MEM_OBJECT_IMAGE2D;
    image_desc.image_width = image_width;
    image_desc.image_height = image_height;
    image_desc.image_depth = 1;
    image_desc.image_array_size = 1;
    image_desc.image_row_pitch = 0;
    image_desc.image_slice_pitch = 0;
    image_desc.num_mip_levels = 0;
    image_desc.num_samples = 0;
    image_desc.mem_object = NULL;

    cl_mem cl_image = clCreateImage(cl_object.m_context, CL_MEM_WRITE_ONLY, &image_format, &image_desc, NULL, &err);
    if (err < 0) {
        ERROR("Can not create image object " << err);
    }
    cl_object.set_kernel_arg(0, sizeof(cl_mem), &cl_image);

    char NULL_wrapper = '\n';
    // Camera settings

    Camera cam(lookfrom, lookat, vup, 40, aspect_ratio, aperture, dist_to_focus, 0.0f, 1.0f);

    CL_Camera camcl;
    cam.get_cl_structure(&camcl);

    cl_mem cam_mem = cl_object.create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(CL_Camera), &camcl);
    cl_object.set_kernel_arg(1, sizeof(cl_mem), &cam_mem);

    cl_mem sferki = cl_object.create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, ptr_spheres_size == 0 ? 1 : ptr_spheres_size, ptr_spheres == nullptr ? &NULL_wrapper : ptr_spheres);
    cl_object.set_kernel_arg(2, sizeof(cl_mem), &sferki);

    // MATERIAŁY
    // ----------

    cl_mem albedo_mem = cl_object.create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, ptr_albedo_size == 0 ? 1 : ptr_albedo_size, ptr_albedo == nullptr ? &NULL_wrapper : ptr_albedo);
    cl_object.set_kernel_arg(3, sizeof(cl_mem), &albedo_mem);

    cl_mem metal_mem = cl_object.create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, ptr_metal_size == 0 ? 1 : ptr_metal_size, ptr_metal == nullptr ? &NULL_wrapper : ptr_metal);
    cl_object.set_kernel_arg(4, sizeof(cl_mem), &metal_mem);

    cl_mem fuzz_mem = cl_object.create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, ptr_fuzz_size == 0 ? 1 : ptr_fuzz_size, ptr_fuzz == nullptr ? &NULL_wrapper : ptr_fuzz);
    cl_object.set_kernel_arg(5, sizeof(cl_mem), &fuzz_mem);

    // // Moving Spheres

    cl_mem moving_s = cl_object.create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, ptr_moving_spheres_size == 0 ? 1 : ptr_moving_spheres_size, ptr_moving_spheres == nullptr ? &NULL_wrapper : ptr_moving_spheres);
    cl_object.set_kernel_arg(6, sizeof(cl_mem), &moving_s);

    // BVH

    cl_mem BVH_kernel_mem = cl_object.create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, ptr_BVH_size == 0 ? 1 : ptr_BVH_size, ptr_BVH == nullptr ? &NULL_wrapper : ptr_BVH);
    cl_object.set_kernel_arg(7, sizeof(cl_mem), &BVH_kernel_mem);

    // TEKSTURY!!!!!!!!

    cl_mem solid_mem = cl_object.create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, ptr_texture_solid_size == 0 ? 1 : ptr_texture_solid_size, ptr_texture_solid == nullptr ? &NULL_wrapper : ptr_texture_solid);
    cl_object.set_kernel_arg(8, sizeof(cl_mem), &solid_mem);

    std::cout << "PUSZCZAM KERNEL" << std::endl;

    size_t global_size[2] = {image_height, image_width};
    err = clEnqueueNDRangeKernel(cl_object.m_queue, cl_object.m_kernel, 2, NULL, global_size, NULL, 0, NULL, NULL);
    if (err < 0) {
        ERROR("Can not enqueue kernle " << err);
    }
    
    size_t pixels_num = 4 * image_width * image_height;
    char* pixels = new char [pixels_num];

    size_t origin[3], region[3];
    origin[0] = 0; 
    origin[1] = 0; 
    origin[2] = 0;
    
    region[0] = image_width; 
    region[1] = image_height; 
    region[2] = 1;

    err = clEnqueueReadImage(cl_object.m_queue, cl_image, CL_TRUE, origin, region, 0, 0, (void*)pixels, 0, NULL, NULL);
    if(err < 0) {
        perror("Couldn't read from the image object");
        exit(1);   
    }

    stbi_flip_vertically_on_write(true);
    stbi_write_png("result.png", image_width, image_height, 4, pixels, image_width * 4 * sizeof(char));
    free(pixels);

    /* Cleaning */

    std::free(ptr_albedo);
    std::free(ptr_metal);
    std::free(ptr_fuzz);
    std::free(ptr_spheres);
    std::free(ptr_moving_spheres);

    return 0;
}