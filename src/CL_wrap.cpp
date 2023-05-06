#include "CL_wrap.hpp"

#include "error.h"
#include "utility_functions.hpp"

namespace CL
{
    CL_wrap::CL_wrap()
        : m_build_options{""}
    {
        cl_int err;

        err = clGetPlatformIDs(1, &m_platform, NULL);
        if (err < 0) {
            ERROR("Any platform has not been detected, do you have any OpenCL SDK installed? err = " << err);
        }

        err = clGetDeviceIDs(m_platform, CL_DEVICE_TYPE_GPU, 1, &m_device, NULL);
        if (err < 0) {
            ERROR("Any GPU device related to previously found platform has not been detected; err = " << err);
        }

        m_context = clCreateContext(NULL, 1, &m_device, NULL, NULL, &err);
        if (err < 0) {
            ERROR("Context can not be created: err = " << err);
        }

        m_queue = clCreateCommandQueueWithProperties(m_context, m_device, NULL, &err);
        if(err < 0) {
            ERROR("Comand queue can not be created: err = " << err);
        }
    }

    CL_wrap::~CL_wrap()
    {
        clReleaseKernel(m_kernel);
        clReleaseProgram(m_program);
        clReleaseCommandQueue(m_queue);
        clReleaseContext(m_context);
        clReleaseDevice(m_device);
    }

    void CL_wrap::create_program(std::string program_file)
    {
        cl_int err;

        size_t program_size;
        char* program_buffer = read_file(program_file.c_str(), &program_size);
        if (!program_buffer) {
            ERROR("Can not read a file \"" << program_file << "\"");
        }

        m_program = clCreateProgramWithSource(m_context, 1, (const char**)&program_buffer, &program_size, &err);
        if(err < 0) {
            ERROR("Can not create a program from the source file");
        }
        free(program_buffer);
    }

    void CL_wrap::build_program()
    {
        cl_int err;

        err = clBuildProgram(m_program, 0, NULL, m_build_options.c_str(), NULL, NULL);
        if(err < 0) {
            /* Find size of log and print to std output */
            size_t log_size;
            char* program_log;

            clGetProgramBuildInfo(m_program, m_device, CL_PROGRAM_BUILD_LOG,  0, NULL, &log_size);
            program_log = (char*) malloc(log_size + 1);
            program_log[log_size] = '\0';
            clGetProgramBuildInfo(m_program, m_device, CL_PROGRAM_BUILD_LOG, log_size + 1, program_log, NULL);
            printf("%s\n", program_log);
            free(program_log);
            exit(1);
        }
    }

    void CL_wrap::create_kelner(std::string kelner_name)
    {
        cl_int err;

        m_kernel = clCreateKernel(m_program, kelner_name.c_str(), &err);
        if(err < 0) {
            ERROR("Can not create a kelner \"" << kelner_name << "\" err = " << err);
        }
    }

    void CL_wrap::add_include_dir(std::string Idir)
    {
        m_build_options += " -I" + Idir;
    }

    void CL_wrap::add_define(std::string DEFINE_NAME, std::string VALUE)
    {
        m_build_options += " -D " + DEFINE_NAME + "=" + VALUE;
    }

    const std::string& CL_wrap::get_build_options() const
    {
        return m_build_options;
    }


}