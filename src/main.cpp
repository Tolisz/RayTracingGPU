#define CL_TARGET_OPENCL_VERSION 300

#ifdef MAC
#include <OpenCL/cl.h>
#else  
#include <CL/cl.h>
#endif

#include <iostream>
#include <cmath>

#include "error.h"
#include "utility_functions.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define NUMBER_OF_SPHERES 6
#define SAMPLES_PER_PIXEL 100
#define MAX_RECURSION_DEPTH 50

#include "clwrapper.hpp"
#include "camera.hpp"

typedef struct _Spheres_World
{
    // coordinates
    cl_float x[NUMBER_OF_SPHERES];
    cl_float y[NUMBER_OF_SPHERES];
    cl_float z[NUMBER_OF_SPHERES];

    // radius
    cl_float r[NUMBER_OF_SPHERES];

    // materials
    cl_int material[NUMBER_OF_SPHERES];
} 
Spheres_World;


// Material 
// 0 - lambertian
// 1 - metal 
// 2 - dielectric
typedef struct _Materials
{
    cl_float3 albedo[NUMBER_OF_SPHERES];    // 0, 1 
    cl_float fuzz[NUMBER_OF_SPHERES];       // 1
    cl_float ir[NUMBER_OF_SPHERES];         // 2
} 
Materials;

#include "vec/vec.hpp"

//  --------------------------------  //
//                MAIN                //
//  --------------------------------  // 

int main(int argc, char** argv)
{
    CL::CLwrapper cl_wrapper;

    cl_int err;

    /* OpenCL program and kelner*/

    std::string program_file = "src_opencl/kelner.cl";
    std::string kelner_name = "ray_tracer";

    size_t program_size;
    char* program_buffer = read_file(program_file.c_str(), &program_size);
    if (!program_buffer) {
        ERROR("Can not read a file \"" << program_file << "\"");
    }

    cl_program program;
    program = clCreateProgramWithSource(cl_wrapper.context, 1, (const char**)&program_buffer, &program_size, &err);
    if(err < 0) {
        ERROR("Can not create a program from the source file");
    }
    free(program_buffer);

    std::string build_options = "-I./src_opencl "; 
    build_options += "-D NUMBER_OF_SPHERES=" + std::to_string(NUMBER_OF_SPHERES); 
    build_options += " -D SAMPLES_PER_PIXEL=" + std::to_string(SAMPLES_PER_PIXEL);
    build_options += " -D MAX_RECURSION_DEPTH=" + std::to_string(MAX_RECURSION_DEPTH);
    err = clBuildProgram(program, 0, NULL, build_options.c_str(), NULL, NULL);
    if(err < 0) {
        /* Find size of log and print to std output */
        size_t log_size;
        char* program_log;

        clGetProgramBuildInfo(program, cl_wrapper.device, CL_PROGRAM_BUILD_LOG,  0, NULL, &log_size);
        program_log = (char*) malloc(log_size + 1);
        program_log[log_size] = '\0';
        clGetProgramBuildInfo(program, cl_wrapper.device, CL_PROGRAM_BUILD_LOG, log_size + 1, program_log, NULL);
        printf("%s\n", program_log);
        free(program_log);
        exit(1);
    }

    /* OpenCL kernel */

    cl_kernel kernel = clCreateKernel(program, kelner_name.c_str(), &err);
    if(err < 0) {
        ERROR("Can not create a kelner \"" << kelner_name << "\" err = " << err);
    }

    /* Image parametrs and OpenCL image object creation */

    // Camera class parametrs
    float aspect_ratio = 9.0f / 16.0f;

    size_t image_width = 400;
    size_t image_height = image_width * aspect_ratio;

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

    cl_mem cl_image = clCreateImage(cl_wrapper.context, CL_MEM_WRITE_ONLY, &image_format, &image_desc, NULL, &err);
    if (err < 0) {
        ERROR("Can not create image object " << err);
    }

    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &cl_image);
    if (err < 0) {
        ERROR("Can not set Kernel Argument " << err);
    }

    // Camera settings
    

    vec::vec3 lookfrom(3,3,2);
    vec::vec3 lookat(0,0,-1);
    vec::vec3 vup(0,1,0);
    auto dist_to_focus = (lookfrom-lookat).length();
    auto aperture = 0.1;

    Camera cam(lookfrom, lookat, vup, 40, aspect_ratio, aperture, dist_to_focus);

    CL_Camera camcl;
    cam.get_cl_structure(&camcl);

    err = clSetKernelArg(kernel, 1, sizeof(camcl), &camcl);
    if (err < 0) {
        ERROR("Can not set Kernel Argument " << err);
    }

    Spheres_World test_sphere;
    test_sphere.x[0] = 0.0f;
    test_sphere.y[0] = 0.0f;
    test_sphere.z[0] = -1.0f;
    test_sphere.r[0] = 0.5f;
    test_sphere.material[0] = 0;

    test_sphere.x[1] = 0.0f;
    test_sphere.y[1] = -100.5f;
    test_sphere.z[1] = -1.0f;
    test_sphere.r[1] = 100.0f;
    test_sphere.material[1] = 0;

    test_sphere.x[2] = -1.0f;
    test_sphere.y[2] = 0.0f;
    test_sphere.z[2] = -1.0f;
    test_sphere.r[2] = 0.5f;
    test_sphere.material[2] = 2;

    test_sphere.x[3] = 1.0f;
    test_sphere.y[3] = 0.0f;
    test_sphere.z[3] = -1.0f;
    test_sphere.r[3] = 0.5f;
    test_sphere.material[3] = 1;

    test_sphere.x[4] = -1.0f;
    test_sphere.y[4] = 0.0f;
    test_sphere.z[4] = -1.0f;
    test_sphere.r[4] = -0.4f;
    test_sphere.material[4] = 2;


    Materials materials;
    materials.albedo[0] = {0.7f, 0.3f, 0.3f};
    materials.albedo[1] = {0.8f, 0.8f, 0.8f};

    materials.albedo[2] = {0.8f, 0.8f, 0.8f};
    //materials.fuzz[2] = 0.3f;
    materials.ir[2] = 1.5f;

    materials.albedo[3] = {0.8f, 0.6f, 0.2f};
    materials.fuzz[3] = 0.4f;

    materials.ir[4] = 1.5f;

    err = clSetKernelArg(kernel, 2, sizeof(test_sphere), &test_sphere);
    if (err < 0) {
        ERROR("Can not set Kernel Argument " << err);
    }

    std::cout << "sizeof(materials) = " << sizeof(materials) << std::endl;

    err = clSetKernelArg(kernel, 3, sizeof(materials), &materials);
    if (err < 0) {
        ERROR("Can not set Kernel Argument " << err);
    }

    size_t global_size[2] = {image_height, image_width};
    err = clEnqueueNDRangeKernel(cl_wrapper.queue, kernel, 2, NULL, global_size, NULL, 0, NULL, NULL);
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

    err = clEnqueueReadImage(cl_wrapper.queue, cl_image, CL_TRUE, origin, region, 0, 0, (void*)pixels, 0, NULL, NULL);
    if(err < 0) {
        perror("Couldn't read from the image object");
        exit(1);   
    }

    stbi_flip_vertically_on_write(true);
    stbi_write_png("result.png", image_width, image_height, 4, pixels, image_width * 4 * sizeof(char));
    free(pixels);

    /* Cleaning */

    clReleaseKernel(kernel);
    clReleaseProgram(program);

    return 0;
}