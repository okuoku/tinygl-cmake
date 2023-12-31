cmake_minimum_required(VERSION 3.5)
project(TinyGL-cmake C)

set(srcs0
    api.c list.c vertex.c init.c matrix.c texture.c 
    misc.c clear.c light.c clip.c select.c get.c error.c 
    zbuffer.c zline.c zdither.c ztriangle.c 
    zmath.c image_util.c oscontext.c msghandling.c 
    arrays.c specbuf.c memory.c
    )

set(srcs)
foreach(e ${srcs0})
    list(APPEND srcs TinyGL/src/${e})
endforeach()


add_library(TinyGL STATIC
    ${srcs}
    )

target_include_directories(TinyGL PUBLIC TinyGL/include)
