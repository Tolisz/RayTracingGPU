#include "clwrapper.hpp"

#include "error.h"

namespace CL {

    CLwrapper::CLwrapper()
    {
        /* OpenCL platform */

        platform;
        err = clGetPlatformIDs(1, &platform, NULL);
        if (err < 0) {
            ERROR("Any platform has not been detected, do you have any OpenCL SDK installed? err = " << err);
        }

        /* OpenCL device */
        
        device;
        err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
        if (err < 0) {
            ERROR("Any device related to previously found platform has not been detected; err = " << err);
        }

        /* OpenCL Context */
        
        context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
        if (err < 0) {
            ERROR("Context can not be created: err = " << err);
        }

        /* OpenCL comand queue */

        queue = clCreateCommandQueueWithProperties(context, device, NULL, &err);
        if(err < 0) {
            ERROR("Comand queue can not be created: err = " << err);
        }
    }

    CLwrapper::~CLwrapper()
    {
        clReleaseCommandQueue(queue);
        clReleaseContext(context);
        clReleaseDevice(device);
    }


}   // namespace CL