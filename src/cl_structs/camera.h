#pragma once

#define CL_TARGET_OPENCL_VERSION 300

#ifdef MAC
#include <OpenCL/cl.h>
#else  
#include <CL/cl.h>
#endif

typedef struct _CL_Camera
{
    cl_float3 origin;
    cl_float3 lower_left_corner;
    cl_float3 horizontal;
    cl_float3 vertical;
} 
CL_Camera;