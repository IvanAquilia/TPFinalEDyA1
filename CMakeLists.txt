cmake_minimum_required(VERSION 3.31)
project(TPFinalEDyA C)

set(CMAKE_C_STANDARD 11)

add_executable(TPFinalEDyA main.c
        parser.c
        parser.h
        glist.c
        glist.h
        hash.c
        hash.h
        funciones.c
        funciones.h
        utils.c
        utils.h
        listas.c
        listas.h
        garray.c
        garray.h
        string_utils.c
        string_utils.h)
