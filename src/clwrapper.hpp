#pragma once

#define CL_TARGET_OPENCL_VERSION 300

#ifdef MAC
#include <OpenCL/cl.h>
#else  
#include <CL/cl.h>
#endif

namespace CL {

    class CLwrapper 
    {
    public:

        CLwrapper();
        ~CLwrapper();

    public:

        cl_platform_id platform;
        cl_device_id device;
        cl_context context;
        cl_command_queue queue;

        cl_int err;
    };

} // namespace CL