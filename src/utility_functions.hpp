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
char* read_file(const char* file_path, size_t* file_size);

#endif