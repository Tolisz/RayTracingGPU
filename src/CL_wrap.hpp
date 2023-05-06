#pragma once

#define CL_TARGET_OPENCL_VERSION 300

#ifdef MAC
#include <OpenCL/cl.h>
#else  
#include <CL/cl.h>
#endif

#include <string>
#include <list>

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
        
        void create_kernel(std::string kelner_name);
        void set_kernel_arg(cl_uint arg_index, size_t arg_size, const void *arg_value);
        
        void add_include_dir(std::string dir);
        void add_define(std::string DEFINE_NAME, std::string VALUE);

        cl_mem create_buffer(cl_mem_flags flags, size_t size, void *host_ptr);

        const std::string& get_build_options() const;


    public:

        cl_platform_id      m_platform;
        cl_device_id        m_device;
        cl_context          m_context;
        cl_command_queue    m_queue;

        cl_program          m_program;
        cl_kernel           m_kernel;

        std::string         m_build_options;

    private:

        std::list<cl_mem>   m_buffer_list;
    };

}