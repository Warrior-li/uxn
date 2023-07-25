//
// Created by Warrior on 22/07/2023.
//

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#include <unistd.h>
#else
#include <CL/cl.h>
#endif

int opencl_init(cl_platform_id *platformId,
                cl_uint *numPlatforms,
                cl_uint *numDevices,
                cl_device_id * deviceId,
                cl_context *context,
                cl_command_queue * queue);