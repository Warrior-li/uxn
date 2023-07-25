//
// Created by Warrior on 22/07/2023.
//

#include "gpu.h"

int
opencl_init(cl_platform_id *platformId,
            cl_uint *numPlatforms,
            cl_uint *numDevices,
            cl_device_id * deviceId,
            cl_context *context,
            cl_command_queue * queue){


    cl_int ret;

    // Set up platform and GPU device
    ret = clGetPlatformIDs(1, platformId, numPlatforms);

    ret = clGetDeviceIDs(*platformId, CL_DEVICE_TYPE_DEFAULT, 1, deviceId, numDevices);

    // Create an OpenCL context
    *context = clCreateContext(0, 1, deviceId, NULL, NULL, &ret);

    // Create a command queue
    *queue = clCreateCommandQueue(*context, *deviceId, 0, &ret);


    return 1;
}