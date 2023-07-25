//
// Created by Warrior on 22/07/2023.
//

#include <stdio.h>
#include <stdlib.h>

#include "../uxn.h"
#include "system.h"

char * getKernelSource(char *filename);
char *err_code (cl_int err_in);

int system_error(char *msg, const char *err){
    fprintf(stderr, "%s: %s\n", msg, err);
    fflush(stderr);
    return 0;
}

int
system_load(Uxn *u, char *filename,cl_context context, cl_command_queue queue, cl_device_id deviceId, cl_mem memU)
{

    Uint8 *ram = (Uint8 *)calloc(0x10000 * RAM_PAGES, sizeof(Uint8));
    int l, i = 0;
    FILE *f = fopen(filename, "rb");
    if(!f)
        return 0;
    l = fread(&ram[PAGE_PROGRAM], 0x10000 - PAGE_PROGRAM, 1, f);
    while(l && ++i < RAM_PAGES)
        l = fread(&ram + 0x10000 * i, 0x10000, 1, f);
    fclose(f);

    char * kernelSource;

    cl_int ret;
    cl_mem memRam = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(Uint8) * 0x10000 * RAM_PAGES, NULL, &ret);
    // Write into compute device memory
    ret = clEnqueueWriteBuffer(queue, memRam, CL_TRUE, 0, sizeof(Uint8) * 0x10000 * RAM_PAGES, ram, 0, NULL, NULL);


    kernelSource = getKernelSource("./system.cl");
    cl_program program = clCreateProgramWithSource(context, 1, (const char **)&kernelSource, NULL, &ret);
    free(kernelSource);
    ret = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);

    if (ret != CL_SUCCESS)
    {
        size_t len;
        char buffer[2048];

        printf("Error: Failed to build program executable!\n%s\n", err_code(ret));
        clGetProgramBuildInfo(program, deviceId, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
        printf("%s\n", buffer);
        return EXIT_FAILURE;
    }

    // Create the OpenCL kernel
    cl_kernel kernel = clCreateKernel(program, "system_load", &ret);

    ret = clSetKernelArg(kernel, 0 , sizeof(cl_mem),(void *)&memU);
    ret |= clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&memRam);

    size_t globalSize = 0x10000 * RAM_PAGES;
    ret = clEnqueueNDRangeKernel(queue, kernel, 1 ,NULL, &globalSize, NULL,0,NULL,NULL);


    ret = clEnqueueReadBuffer(queue, memU, CL_TRUE, 0 ,sizeof(Uxn)*1+sizeof(Uint8)*0x10000*RAM_PAGES,u,0,NULL,NULL);

    for (int j = 0; j < 10; ++j) {
        printf("%x\n",u->ram[PAGE_PROGRAM+j]);
    }

    return 1;
}

