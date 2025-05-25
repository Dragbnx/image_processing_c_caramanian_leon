cmake_minimum_required(VERSION 3.30)
project(image_processing_c_caramanian_leon C)

set(CMAKE_C_STANDARD 11)

add_executable(image_processing_c_caramanian_leon main.c bmp24.c bmp8.c)
