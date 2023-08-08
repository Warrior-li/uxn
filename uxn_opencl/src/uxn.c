#include "uxn.h"

# include <stdio.h>

int output_times = 0;

#define T s->dat[s->ptr - 1]
#define N s->dat[s->ptr - 2]
#define L s->dat[s->ptr - 3]


#define HALT(c) { return uxn_halt(u, ins, (c), *pc - 1); }
#define PUT(o, v) { s->dat[(Uint8)(s->ptr - 1 - (o))] = (v); }
#define SET(mul, add) { if(mul > s->ptr) HALT(1) tmp = (mul & k) + add + s->ptr; if(tmp > 254) HALT(2) s->ptr = tmp; }

#define DEO(a, b) { ++output_times; u->dev[(a)] = (b); if((deo_mask[(a) >> 4] >> ((a) & 0xf)) & 0x1) uxn_deo(u, (a)); }
#define DEI(a, b) { PUT((a), ((dei_mask[(b) >> 4] >> ((b) & 0xf)) & 0x1) ? uxn_dei(u, (b)) : u->dev[(b)]) }


int uxn_boot(
        Uxn *u,
        Uint8 *ram){

    u->ram = ram;


    cl_kernel kernel = NULL;
    cl_int ret;

    cl_uint num_devices;
    clGetContextInfo(env.context, CL_CONTEXT_NUM_DEVICES, sizeof(cl_uint), &num_devices, NULL);


    uxn_m.dev = clCreateBuffer(env.context, CL_MEM_READ_WRITE,sizeof(u->dev), NULL,&ret);

    uxn_m.wst = clCreateBuffer(env.context, CL_MEM_READ_WRITE, sizeof(u->wst), NULL,&ret);
    uxn_m.rst = clCreateBuffer(env.context, CL_MEM_READ_WRITE, sizeof(u->rst), NULL,&ret);
    uxn_m.ram = clCreateBuffer(env.context, CL_MEM_READ_WRITE, 0x10000 * 0x10 * sizeof(Uint8), NULL,&ret);

    write_uxn_to_gpu(&env,u,&uxn_m);


    ret = create_kernel("../src/uxn.cl","uxn_boot",&kernel,&env);


    ret = clSetKernelArg(kernel, 0 , sizeof(cl_mem),&uxn_m.dev);
    ret |= clSetKernelArg(kernel, 1 , sizeof(cl_mem),&uxn_m.wst);
    ret |= clSetKernelArg(kernel, 2 , sizeof(cl_mem),&uxn_m.rst);


    size_t globalSize = sizeof(Stack) ;
    ret = clEnqueueNDRangeKernel(env.queue, kernel, 1 ,NULL, &globalSize, NULL,0,NULL,NULL);


    return 1;
}

int uxn_eval_cpu(Uxn *u, Uint16 *pc){
    int t, n, l, k, tmp, opc, ins;
    Uint8 *ram = u->ram;
    Stack *s;
    if(!pc || u->dev[0x0f]) return 0;

    ins = ram[*pc] & 0xff;
    *pc = *pc + 1;

    k = ins & 0x80 ? 0xff : 0;
    s = ins & 0x40 ? &u->rst : &u->wst;
    opc = !(ins & 0x1f) ? (0 - (ins >> 5)) & 0xff : ins & 0x3f;


    switch(opc) {
        /* IMM */
        case 0x00: /* BRK   */ return 0;
        case 0x16: /* DEI  */ t=T;            SET(1, 0) DEI(0, t) return 1;
        case 0x36:            t=T;            SET(1, 1) DEI(1, t) DEI(0, t + 1) return 1;
        case 0x17: /* DEO  */ t=T;n=N;        SET(2,-2) DEO(t, n) return 1;
        case 0x37:            t=T;n=N;l=L;    SET(3,-3) DEO(t, l) DEO(t + 1, n) return 1;
    }
    return 1;
}

int halt_cpu(Uxn *u, Uint16 *pc, char halt){
    Uint8 *ram = u->ram;
    int ins = ram[*pc++] & 0xff;
    HALT(halt)
}

int uxn_eval(Uxn *u,
             Uint16 pc)
{


    cl_kernel kernel = NULL;
    cl_int ret;

    cl_mem mem_pc = clCreateBuffer(env.context, CL_MEM_READ_WRITE,sizeof(Uint16), NULL,&ret);
    ret = clEnqueueWriteBuffer(env.queue,mem_pc,CL_TRUE,0,sizeof(Uint16),&pc,0,NULL,NULL);

    Uint16 cord[100];
    Uint8 result=0;
    Uint8 halt=0;
    cl_mem mem_cord = clCreateBuffer(env.context, CL_MEM_READ_WRITE,sizeof(Uint16)*100, NULL,&ret);
    cl_mem mem_re = clCreateBuffer(env.context, CL_MEM_READ_WRITE,sizeof(Uint8), NULL,&ret);
    cl_mem mem_halt = clCreateBuffer(env.context, CL_MEM_READ_WRITE,sizeof(Uint8), NULL,&ret);

    ret = clEnqueueWriteBuffer(env.queue,mem_re,CL_TRUE,0,sizeof(Uint8),&result,0,NULL,NULL);
    ret = clEnqueueWriteBuffer(env.queue,mem_halt,CL_TRUE,0,sizeof(Uint8),&halt,0,NULL,NULL);



    ret = create_kernel("../src/uxn.cl","uxn_eval",&kernel,&env);

    ret = clSetKernelArg(kernel, 0 , sizeof(cl_mem),&uxn_m.ram);
    ret |= clSetKernelArg(kernel, 1 , sizeof(cl_mem),&uxn_m.wst);
    ret |= clSetKernelArg(kernel, 2 , sizeof(cl_mem),&uxn_m.rst);
    ret |= clSetKernelArg(kernel, 3 , sizeof(cl_mem),&uxn_m.dev);
    ret |= clSetKernelArg(kernel, 4 , sizeof(cl_mem),&mem_pc);
    ret |= clSetKernelArg(kernel, 5, sizeof (cl_mem),&mem_re);
    ret |= clSetKernelArg(kernel, 6, sizeof(cl_mem), &mem_halt);
    ret |= clSetKernelArg(kernel, 7, sizeof(cl_mem), &mem_cord);





    size_t globalSize = 1;
    int times = 0;
    for(;;){
        ++times;

        if(write_uxn_to_gpu(&env,u,&uxn_m))break;

        ret = clEnqueueNDRangeKernel(env.queue, kernel, 1 ,NULL, &globalSize, NULL,0,NULL,NULL);

        read_uxn_from_gpu(&env,u,&uxn_m);

        ret = clEnqueueReadBuffer(env.queue, mem_pc , CL_TRUE, 0 ,sizeof(Uint16),&pc,0,NULL,NULL);
        ret = clEnqueueReadBuffer(env.queue, mem_halt , CL_TRUE, 0 ,sizeof(Uint8),&halt,0,NULL,NULL);
        ret = clEnqueueReadBuffer(env.queue, mem_re , CL_TRUE, 0 ,sizeof(Uint8),&result,0,NULL,NULL);

        clFinish(env.queue);


        if(halt != 0) { return halt_cpu(u,&pc,halt);}
        if(!uxn_eval_cpu(u,&pc)) break;

        ret = clEnqueueWriteBuffer(env.queue,mem_halt,CL_TRUE,0,sizeof(Uint8),&halt,0,NULL,NULL);
        ret = clEnqueueWriteBuffer(env.queue,mem_pc,CL_TRUE,0,sizeof(Uint16),&pc,0,NULL,NULL);
        ret = clEnqueueWriteBuffer(env.queue,mem_re,CL_TRUE,0,sizeof(Uint8),&result,0,NULL,NULL);

    }

    ret = clEnqueueReadBuffer(env.queue, mem_cord , CL_TRUE, 0 ,sizeof(Uint16)*100,&cord,0,NULL,NULL);


    printf("DEO: %d\n",output_times);
    return 1;

}
