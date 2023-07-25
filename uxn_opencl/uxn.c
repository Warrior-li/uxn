

#include <stdio.h>
#include <stdlib.h>
#include "uxn.h"
#include "err_code.h"

char * getKernelSource(char *filename);

int uxn_boot(Uxn *u, Uint8 *ram, cl_context context, cl_command_queue queue, cl_device_id deviceId, cl_mem memU, int ram_pages){

    char * kernelSource;    // kernel source string

    cl_mem memRam = NULL;
    cl_program program = NULL;
    cl_kernel kernel = NULL;
    cl_int ret;

    // Create memory buffers for input and output
    memRam = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(Uint8) * 0x10000 * ram_pages,NULL,&ret);

    ret = clEnqueueWriteBuffer(queue,memU,CL_TRUE,0,sizeof(Uxn),u,0,NULL,NULL);
    ret = clEnqueueWriteBuffer(queue,memRam,CL_TRUE,0,sizeof(Uint8) * 0x10000 * ram_pages,ram,0,NULL,NULL);

    // Create a program from the kernel source
    kernelSource = getKernelSource("./uxn.cl");
    program = clCreateProgramWithSource(context, 1, (const char **)&kernelSource, NULL, &ret);
    checkError(ret, "Creating program with uxn.cl");

    free(kernelSource);

    // Build the program
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
    kernel = clCreateKernel(program, "uxn_boot", &ret);

    checkError(ret, "Creating kernel from uxn.cl");

    ret = clSetKernelArg(kernel, 0 , sizeof(cl_mem),(void *)&memU);
    ret |= clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&memRam);

    checkError(ret, "Setting kernel args");

    size_t globalSize = 0x10000 * ram_pages;
    ret = clEnqueueNDRangeKernel(queue, kernel, 1 ,NULL, &globalSize, NULL,0,NULL,NULL);
    checkError(ret, "Enqueueing kernel");

    //ret = clEnqueueReadBuffer(queue, memU, CL_TRUE, 0 ,sizeof(Uxn)*1,u,0,NULL,NULL);

    return 1;
}

char * getKernelSource(char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        fprintf(stderr, "Error: Could not open kernel source file\n");
        exit(EXIT_FAILURE);
    }
    fseek(file, 0, SEEK_END);
    int len = ftell(file) + 1;
    rewind(file);

    char *source = (char *)calloc(sizeof(char), len);
    if (!source)
    {
        fprintf(stderr, "Error: Could not allocate memory for source string\n");
        exit(EXIT_FAILURE);
    }
    fread(source, sizeof(char), len, file);
    fclose(file);
    return source;
}