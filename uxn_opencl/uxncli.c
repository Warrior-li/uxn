//
// Created by Warrior on 22/07/2023.
//
#include <stdio.h>
#include <stdlib.h>

#include "uxn.h"
#include "devices/system.h"
#include "devices/gpu.h"



Uint16 deo_mask[] = {0xc028, 0x0300, 0xc028, 0x8000, 0x8000, 0x8000, 0x8000, 0x0000, 0x0000, 0x0000, 0xa260, 0xa260, 0x0000, 0x0000, 0x0000, 0x0000};
Uint16 dei_mask[] = {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x07ff, 0x0000, 0x0000, 0x0000};


int main(int argc, char **argv){
    Uxn u;
    int i = 1;
    if(i == argc) return system_error("usage", "uxncli file.rom [args..]");

    double start_time;
    double run_time;

    cl_platform_id platformId = NULL;
    cl_device_id deviceId = NULL;
    cl_context context = NULL;
    cl_command_queue queue = NULL;
    cl_uint numDevices, numPlatforms;
    cl_mem memU = NULL;





    if(!opencl_init(&platformId,&numPlatforms,&numDevices,&deviceId, &context,&queue))
        return system_error("opencl_init","OpenCL init failed");

    memU = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(Uxn)*1+sizeof(Uint8)*0x10000*RAM_PAGES, NULL, NULL);

    if(!uxn_boot(&u,(Uint8 *)calloc(0x10000 * RAM_PAGES, sizeof(Uint8)),context,queue,deviceId, memU,RAM_PAGES))
        return system_error("Boot", "Failed");

    if(!system_load(&u, argv[i++],context,queue,deviceId,memU))
        return system_error("Load", "Failed");

    u.dev[0x17] = argc - i;

}

