#ifndef UTILITY_FUNCTIONS
#define UTILITY_FUNCTIONS

#include <iostream>
#include <stdio.h>

#include <errno.h>
#include <string.h>
#include "error.h"

//
//  @brief function for text file reading
//  @param file_path - path to a text file to be opened
//  @param file_size - size of file to be opened. Can be set to NULL if file's size is unimportant 
//  @return buffor containing file characters.
// 
char* read_file(const char* file_path, size_t* file_size)
{
    FILE* file_handle = fopen(file_path, "rb");
    if(!file_handle) {
        WARRING("Can not open file \"" << file_path << "\": " << strerror(errno));
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
        WARRING("Can not allocate memory for a file \"" << file_path << "\"");
        fclose(file_handle);
        return nullptr;
    }

    file_buffer[program_size] = '\0';
    fread(file_buffer, sizeof(char), program_size, file_handle);
    fclose(file_handle);

    return file_buffer;
}

#endif