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
#include "utility_functions.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define NUMBER_OF_SPHERES 5
#define SAMPLES_PER_PIXEL 100
#define MAX_RECURSION_DEPTH 50

#include "camera.hpp"

typedef struct _Spheres_World
{
    cl_float3 center[NUMBER_OF_SPHERES];
    cl_float r[NUMBER_OF_SPHERES];

    cl_int mat_id[NUMBER_OF_SPHERES];       // ID materiału który będzie wykorzystywany
    cl_int mat_num[NUMBER_OF_SPHERES];      // Numer materiału w tablicy dla konkretnego ID
} 
Spheres_World;


#include "vec/vec.hpp"
#include "materials.hpp"

//  --------------------------------  //
//                MAIN                //
//  --------------------------------  // 

int main(int argc, char** argv)
{
    cl_int err;

    /* OpenCL platform */

    cl_platform_id platform;
    err = clGetPlatformIDs(1, &platform, NULL);
    if (err < 0) {
        ERROR("Any platform has not been detected, do you have any OpenCL SDK installed? err = " << err);
    }

    /* OpenCL device */
    
    cl_device_id device;
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    if (err < 0) {
        ERROR("Any device related to previously found platform has not been detected; err = " << err);
    }

    /* OpenCL Context */
    
    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    if (err < 0) {
        ERROR("Context can not be created: err = " << err);
    }

    /* OpenCL comand queue */

    cl_command_queue queue = clCreateCommandQueueWithProperties(context, device, NULL, &err);
    if(err < 0) {
        ERROR("Comand queue can not be created: err = " << err);
    }

    /* OpenCL program and kelner*/

    std::string program_file = "src_opencl/kelner.cl";
    std::string kelner_name = "ray_tracer";

    size_t program_size;
    char* program_buffer = read_file(program_file.c_str(), &program_size);
    if (!program_buffer) {
        ERROR("Can not read a file \"" << program_file << "\"");
    }

    cl_program program;
    program = clCreateProgramWithSource(context, 1, (const char**)&program_buffer, &program_size, &err);
    if(err < 0) {
        ERROR("Can not create a program from the source file");
    }
    free(program_buffer);

    std::string build_options = "-I./src_opencl "; 
    build_options += "-D NUMBER_OF_SPHERES=" + std::to_string(NUMBER_OF_SPHERES); 
    build_options += " -D SAMPLES_PER_PIXEL=" + std::to_string(SAMPLES_PER_PIXEL);
    build_options += " -D MAX_RECURSION_DEPTH=" + std::to_string(MAX_RECURSION_DEPTH);
    std::cout << "BUILD OPTIONS = " << build_options << "\n";   
    err = clBuildProgram(program, 0, NULL, build_options.c_str(), NULL, NULL);
    if(err < 0) {
        /* Find size of log and print to std output */
        size_t log_size;
        char* program_log;

        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,  0, NULL, &log_size);
        program_log = (char*) malloc(log_size + 1);
        program_log[log_size] = '\0';
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size + 1, program_log, NULL);
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

    size_t image_width = 600;
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

    cl_mem cl_image = clCreateImage(context, CL_MEM_WRITE_ONLY, &image_format, &image_desc, NULL, &err);
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

    cl_mem cam_mem = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(CL_Camera), &camcl, &err);
    if (err < 0) {
        ERROR("Can not create buffer for camera " << err);
    }

    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &cam_mem);
    if (err < 0) {
        ERROR("Can not set Kernel Argument " << err);
    }

    Spheres_World test_sphere;
    memset(&test_sphere, 0, sizeof(Spheres_World));

    std::cout << "sieof(Spheres_World) = " << sizeof(Spheres_World) << std::endl;
    
    test_sphere.center[0] = {0.0f, 0.0f, -1.0f};
    test_sphere.r[0] = 0.5f;
    test_sphere.mat_id[0] = 0;
    test_sphere.mat_num[0] = 0;

    test_sphere.center[1] = {0.0f, -100.5f, -1.0f};
    test_sphere.r[1] = 100.0f;
    test_sphere.mat_id[1] = 0;
    test_sphere.mat_num[1] = 1;

    test_sphere.center[2] = {-1.0f, 0.0f, -1.0f};
    test_sphere.r[2] = 0.5f;
    test_sphere.mat_id[2] = 2;
    test_sphere.mat_num[2] = 0;

    test_sphere.center[3] = {1.0f, 0.0f, -1.0f};
    test_sphere.r[3] = 0.5f;
    test_sphere.mat_id[3] = 1; 
    test_sphere.mat_num[3] = 0; 

    test_sphere.center[4] = {-1.0f, 0.0f, -1.0f};
    test_sphere.r[4] = -0.4f;
    test_sphere.mat_id[4] = 2;
    test_sphere.mat_num[4] = 1;


    cl_mem sferki = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(Spheres_World), &test_sphere, &err);
    if (err < 0) {
        ERROR("Can not create buffer object" << err);
    }

    err = clSetKernelArg(kernel, 2, sizeof(cl_mem), &sferki);
    if (err < 0) {
        ERROR("Can not set Kernel Argument " << err);
    }

    // MATERIAŁY
    // ----------

    auto albedo1 = std::make_shared<Lambertian>(vec::vec3(0.7f, 0.3f, 0.3f));
    auto albedo2 = std::make_shared<Lambertian>(vec::vec3(0.8f, 0.8f, 0.8f));
    void* ptr_albedo = nullptr;
    size_t ptr_albedo_size;
    Lambertian_List::get_cl_structure(&ptr_albedo, &ptr_albedo_size, nullptr);
    cl_mem albedo_mem = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, ptr_albedo_size, ptr_albedo, &err);
    if (err < 0) {
        ERROR("Can not create buffer" << err);
    }

    auto metal1 = std::make_shared<Metal>(vec::vec3(0.8f, 0.6f, 0.2f), 0.4f);
    void* ptr_metal = nullptr;
    size_t ptr_metal_size;
    Metal_List::get_cl_structure(&ptr_metal, &ptr_metal_size, nullptr);
    cl_mem metal_mem = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, ptr_metal_size, ptr_metal, &err);
    if (err < 0) {
        ERROR("Can not create buffer" << err);
    }
    
    auto fuzz1 = std::make_shared<Dielectric>(1.5f);
    auto fuzz2 = std::make_shared<Dielectric>(1.5f);
    void* ptr_fuzz = nullptr;
    size_t ptr_fuzz_size;
    Dielectric_List::get_cl_structure(&ptr_fuzz, &ptr_fuzz_size, nullptr);
    cl_mem fuzz_mem = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, ptr_fuzz_size, ptr_fuzz, &err);
    if (err < 0) {
        ERROR("Can not create buffer" << err);
    }

    err = clSetKernelArg(kernel, 3, sizeof(cl_mem), &albedo_mem);
    if (err < 0) {
        ERROR("Can not set Kernel Argument " << err);
    }

    err = clSetKernelArg(kernel, 4, sizeof(cl_mem), &metal_mem);
    if (err < 0) {
        ERROR("Can not set Kernel Argument " << err);
    }

    err = clSetKernelArg(kernel, 5, sizeof(cl_mem), &fuzz_mem);
    if (err < 0) {
        ERROR("Can not set Kernel Argument " << err);
    }


    size_t global_size[2] = {image_height, image_width};
    err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global_size, NULL, 0, NULL, NULL);
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

    err = clEnqueueReadImage(queue, cl_image, CL_TRUE, origin, region, 0, 0, (void*)pixels, 0, NULL, NULL);
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

    std::free(ptr_albedo);
    std::free(ptr_metal);

    return 0;
}