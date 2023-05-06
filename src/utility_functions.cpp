#include "utility_functions.hpp"

char* read_file(const char* file_path, size_t* file_size)
{
    FILE* file_handle = fopen(file_path, "rb");
    if(!file_handle) {
        WARNING("Can not open file \"" << file_path << "\": " << strerror(errno));
        return nullptr;
    }

    // Determining file size
    fseek(file_handle, 0, SEEK_END);
    long program_size = ftell(file_handle);
    rewind(file_handle);
    if (file_size) {
        *file_size = program_size;
    } 

    // Read form file
    char* file_buffer = (char*)malloc(program_size + 1);
    if (!file_buffer) {
        WARNING("Can not allocate memory for a file \"" << file_path << "\"");
        fclose(file_handle);
        return nullptr;
    }

    file_buffer[program_size] = '\0';
    fread(file_buffer, sizeof(char), program_size, file_handle);
    fclose(file_handle);

    return file_buffer;
}