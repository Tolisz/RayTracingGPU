#define CL_TARGET_OPENCL_VERSION 300

#ifdef MAC
#include <OpenCL/cl.h>
#else  
#include <CL/cl.h>
#endif

#include <iostream>

#include "error.h"
#include "utility_functions.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

typedef struct _Camera
{
    cl_float viewport_height;
    cl_float viewport_width;
    cl_float focal_length;

    cl_float3 origin;
} Camera;

#define NUMBER_OF_SPHERES 2
#define SAMPLES_PER_PIXEL 100

typedef struct _Spheres_World
{
    // coordinates
    cl_float x[NUMBER_OF_SPHERES];
    cl_float y[NUMBER_OF_SPHERES];
    cl_float z[NUMBER_OF_SPHERES];

    // radius
    cl_float r[NUMBER_OF_SPHERES];
} 
Spheres_World;

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

    std::string program_file = "kelner.cl";
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

    std::string build_options = "-I./cl_include "; 
    build_options += "-D NUMBER_OF_SPHERES=" + std::to_string(NUMBER_OF_SPHERES); 
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

    size_t image_width = 400;
    size_t image_height = (image_width * 9) / 16;

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

    Camera test;
    test.focal_length = 1.0f;
    test.viewport_height = 2.0f;
    test.viewport_width = (2.0f * 16) / 9;
    test.origin = {0.0f, 0.0f, 0.0f};

    err = clSetKernelArg(kernel, 1, sizeof(test), &test);
    if (err < 0) {
        ERROR("Can not set Kernel Argument " << err);
    }

    Spheres_World test_sphere;
    test_sphere.x[0] = 0.0f;
    test_sphere.y[0] = 0.0f;
    test_sphere.z[0] = -1.0f;
    test_sphere.r[0] = 0.5f;

    test_sphere.x[1] = 0.0f;
    test_sphere.y[1] = -100.5f;
    test_sphere.z[1] = -1.0f;
    test_sphere.r[1] = 100.0f;

    err = clSetKernelArg(kernel, 2, sizeof(test_sphere), &test_sphere);
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
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    clReleaseDevice(device);

    return 0;
}