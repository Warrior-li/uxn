#ifdef __APPLE__
#include <OpenCL/opencl.h>
#include <unistd.h>
#else
#include <CL/cl.h>
#endif


int opencl_init(OpenCL_env *env);
char * getKernelSource(char *filename);