#pragma once

#define CL_TARGET_OPENCL_VERSION 300

#ifdef MAC
#include <OpenCL/cl.h>
#else  
#include <CL/cl.h>
#endif

#include <string>

namespace CL
{

    class CL_wrap
    {
    public:

        CL_wrap();
        CL_wrap(const CL_wrap&) = delete;

        ~CL_wrap();

        void create_program(std::string program_file);
        void build_program();
        void create_kelner(std::string kelner_name);
        
        void add_include_dir(std::string dir);
        void add_define(std::string DEFINE_NAME, std::string VALUE);

        const std::string& get_build_options() const;


    public:

        cl_platform_id      m_platform;
        cl_device_id        m_device;
        cl_context          m_context;
        cl_command_queue    m_queue;

        cl_program          m_program;
        cl_kernel           m_kernel;

        std::string         m_build_options;
    };

}