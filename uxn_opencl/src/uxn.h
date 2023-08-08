/*
Copyright (c) 2021 Devine Lu Linvega

Permission to use, copy, modify, and distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE.
*/

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#include <unistd.h>
#else
#include <CL/cl.h>
#endif


#define PAGE_PROGRAM 0x0100

/* clang-format off */

#define POKE2(d, v) { (d)[0] = (v) >> 8; (d)[1] = (v); }
#define PEEK2(d) ((d)[0] << 8 | (d)[1])

/* clang-format on */

typedef unsigned char Uint8;
typedef signed char Sint8;
typedef unsigned short Uint16;
typedef signed short Sint16;
typedef unsigned int Uint32;

typedef struct {
    Uint8 dat[255], ptr;
} Stack;

typedef struct Uxn {
    Uint8 *ram, dev[256];
    Stack wst, rst;
    Uint8 (*dei)(struct Uxn *u, Uint8 addr);
    void (*deo)(struct Uxn *u, Uint8 addr);
} Uxn;

typedef struct Uxn_mem{
    cl_mem ram,dev;
    cl_mem wst,rst;
}Uxn_mem;

typedef struct OpenCL_env{
    cl_platform_id platformId;
    cl_uint numPlatforms;
    cl_uint numDevices;
    cl_device_id deviceId;
    cl_context context;
    cl_command_queue queue;
}OpenCL_env;

Uxn_mem uxn_m;
OpenCL_env env;

/* functions about openCL */

extern int write_uxn_to_gpu(OpenCL_env *env,Uxn *u, Uxn_mem *uxn_m);
extern int read_uxn_from_gpu(OpenCL_env *env,Uxn *u, Uxn_mem *uxn_m);
extern int create_kernel(char *filename,char * fun_nam,cl_kernel *kernel,OpenCL_env *env);
/* required functions */

extern Uint8 uxn_dei(Uxn *u, Uint8 addr);
extern void uxn_deo(Uxn *u, Uint8 addr);
extern int uxn_halt(Uxn *u, Uint8 instr, Uint8 err, Uint16 addr);
extern Uint16 dei_mask[];
extern Uint16 deo_mask[];

/* built-ins */


/*
 * this function flow:
 * 1. Create the memory buffer for all properties(*ram, dev[], wst, rst) of the structure.
 * 2. initial dev[], wst, rst in the OpenCL device
 * 3. initial *ram in the host
 * 4. free some space
 */
int uxn_boot(
        Uxn *u,
        Uint8 *ram);

int uxn_eval(Uxn *u,
             Uint16 pc);


