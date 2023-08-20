#include "../uxn.h"
#include "gpu.h"

#include <stdio.h>
#include "../err_code.h"

int
opencl_init(OpenCL_env *env){

    cl_int ret;

    ret = clGetPlatformIDs(0, NULL, &env->numPlatforms);

    cl_platform_id Platform[env->numPlatforms];

    ret = clGetPlatformIDs(env->numPlatforms, Platform, NULL);

    int i;
    for (i = 0; i < env->numPlatforms; i++)
    {
        ret = clGetDeviceIDs(Platform[i], CL_DEVICE_TYPE_DEFAULT, 1, &env->deviceId, NULL);
        if (ret == CL_SUCCESS)
        {
            break;
        }
    }

    env->context = clCreateContext(0, 1, &env->deviceId, NULL, NULL, &ret);


    env->queue = clCreateCommandQueue(env->context, env->deviceId, 0, &ret);

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




int create_kernel(char *filename,char * fun_nam,cl_kernel *kernel,OpenCL_env *env){
    char * kernelSource;
    cl_program program = NULL;
    cl_int ret;


    kernelSource = getKernelSource(filename);
    program = clCreateProgramWithSource(env->context, 1, (const char **)&kernelSource, NULL, &ret);
    checkError(ret, "Creating program with uxn.cl");
    free(kernelSource);

    ret = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (ret != CL_SUCCESS)
    {
        size_t len;
        char buffer[2048];

        printf("Error: Failed to build program executable!\n%s\n", err_code(ret));
        clGetProgramBuildInfo(program, env->deviceId, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
        printf("%s\n", buffer);
        return EXIT_FAILURE;
    }

    *kernel = clCreateKernel(program, fun_nam, &ret);
    return ret;
}