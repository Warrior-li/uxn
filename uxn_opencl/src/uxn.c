#include "uxn.h"

# include <stdio.h>
# include <string.h>

#define T s->dat[s->ptr - 1]
#define N s->dat[s->ptr - 2]
#define L s->dat[s->ptr - 3]


#define HALT(c) { return uxn_halt(u, ins, (c), *pc - 1); }
#define PUT(o, v) { s.dat[(Uint8)(s.ptr - 1 - (o))] = (v); }
#define SET(mul, add) { if(mul > s->ptr) HALT(1) tmp = (mul & k) + add + s->ptr; if(tmp > 254) HALT(2) s->ptr = tmp; }

#define DEO(a, b) { u->dev[(a)] = (b); if((deo_mask[(a) >> 4] >> ((a) & 0xf)) & 0x1) uxn_deo(u, (a)); }
#define DEI(a, b) { PUT((a), ((dei_mask[(b) >> 4] >> ((b) & 0xf)) & 0x1) ? uxn_dei(u, (b)) : u->dev[(b)]) }

cl_kernel kernel = NULL;
bool createSpace = false;

int
uxn_boot(Uxn *u, Uint8 *ram)
{
    Uint32 i;
    char *cptr = (char *)u;
    for(i = 0; i < sizeof(*u); i++)
        cptr[i] = 0;
    u->ram = ram;
    return 1;
}

int halt_cpu(Uxn *u, Uint16 *pc, char halt){
    Uint8 *ram = u->ram;
    int ins = ram[*pc++] & 0xff;
    HALT(halt)
}


void create_space(){
    create_kernel("../src/uxn.cl","uxn_eval",&kernel,&env);

    mRam = clCreateBuffer(env.context,CL_MEM_READ_WRITE,0x10000 * 0x10 * sizeof(Uint8),NULL,NULL);
    mWst = clCreateBuffer(env.context,CL_MEM_READ_WRITE,sizeof(Stack),NULL,NULL);
    mRst = clCreateBuffer(env.context,CL_MEM_READ_WRITE,sizeof(Stack),NULL,NULL);


    mBufferA = clCreateBuffer(env.context,CL_MEM_READ_WRITE,sizeof(Buffer) * buffer_num,NULL,NULL);
    mBufferB = clCreateBuffer(env.context,CL_MEM_READ_WRITE,sizeof(Buffer) * buffer_num,NULL,NULL);

    mPc = clCreateBuffer(env.context,CL_MEM_READ_WRITE,sizeof(Uint16),NULL,NULL);

    mNum = clCreateBuffer(env.context,CL_MEM_READ_WRITE,sizeof(Uint16),NULL,NULL);

    mFlag = clCreateBuffer(env.context,CL_MEM_READ_WRITE,sizeof(bool),NULL,NULL);

    ptrA = clCreateBuffer(env.context,CL_MEM_READ_WRITE,sizeof(Uint16),NULL,NULL);
    ptrB = clCreateBuffer(env.context,CL_MEM_READ_WRITE,sizeof(Uint16),NULL,NULL);


    clSetKernelArg(kernel, 0 , sizeof(cl_mem),&mRam);
    clSetKernelArg(kernel, 1 , sizeof(cl_mem),&mWst);
    clSetKernelArg(kernel, 2 , sizeof(cl_mem),&mRst);
    clSetKernelArg(kernel, 3 , sizeof(cl_mem),&mPc);
    clSetKernelArg(kernel, 4 , sizeof(cl_mem), &mNum);
    clSetKernelArg(kernel, 6 , sizeof(cl_mem), &mFlag);


}

int uxn_eval(Uxn *u,
             Uint16 pc)
{

    if(!pc || u->dev[0x0f]) return 0;

    if(createSpace == false) {create_space(); createSpace = true;}

    size_t globalSize = 1;

    clEnqueueWriteBuffer(env.queue,mPc,CL_TRUE,0,sizeof(pc),&pc,0,NULL,NULL);

    Uint16 b_num = buffer_num;
    clEnqueueWriteBuffer(env.queue,mNum,CL_TRUE,0,sizeof(b_num),&b_num,0,NULL,NULL);


    bool dei_flag = false;
    clEnqueueWriteBuffer(env.queue,mFlag,CL_TRUE,0,sizeof(bool),&dei_flag,0,NULL,NULL);


    clEnqueueWriteBuffer(env.queue,mWst,CL_TRUE,0,sizeof(Stack),&u->wst,0,NULL,NULL);
    clEnqueueWriteBuffer(env.queue,mRst,CL_TRUE,0,sizeof(Stack),&u->rst,0,NULL,NULL);
    clEnqueueWriteBuffer(env.queue,mRam,CL_TRUE,0,0x10000 * 0x10 * sizeof(Uint8),u->ram,0,NULL,NULL);

    int flag = 0,times = 0;


    cl_event read_buffer,read_ptr;

    bool kernel_end[] = {false,false};

    int i;

    for(;;){

        if(times != 0){
            clEnqueueReadBuffer(env.queue, flag ? mBufferA : mBufferB, CL_FALSE, 0 ,sizeof(Buffer) * buffer_num - 1, flag ? &bufferA.queue : &bufferB.queue,0,NULL,&read_buffer);
            clEnqueueReadBuffer(env.queue, flag ? ptrA : ptrB, CL_FALSE, 0 ,sizeof(Uint16), flag ? &bufferA.ptr : &bufferB.ptr ,0,NULL,&read_ptr);
        }


        if(kernel_end[flag] == false){
            clSetKernelArg(kernel, 5 , sizeof(cl_mem), !flag ? &mBufferA : &mBufferB);
            clSetKernelArg(kernel, 7 , sizeof(cl_mem), !flag ? &ptrA : &ptrB);

            clEnqueueNDRangeKernel(env.queue, kernel, 1 ,NULL, &globalSize, NULL, 0 , NULL , NULL);

        }else if(kernel_end[!flag] == true){
            clEnqueueReadBuffer(env.queue,mWst,CL_TRUE, 0, sizeof(u->wst),&u->wst,0,NULL,NULL);
            clEnqueueReadBuffer(env.queue,mRst,CL_TRUE, 0, sizeof(u->rst),&u->rst,0,NULL,NULL);
            clEnqueueReadBuffer(env.queue,mRam,CL_TRUE, 0, 0x10000 * 0x10 * sizeof(Uint8),u->ram,0,NULL,NULL);
            return 1;
        }


        if(times != 0){

            cl_event waitList[] = {read_buffer,read_ptr};
            clWaitForEvents(2,waitList);

            Buffer_queue buffer = flag ? bufferA : bufferB;

            Stack s;
            int ins = 0;

            for(i = 0; i < buffer.ptr;++i){

                switch (buffer.queue[i].type) {
                    case 4: kernel_end[!flag] = true; break;
                    case 2:
                        DEO(buffer.queue[i].a,buffer.queue[i].b)
                        break;
                    case 1:
                    {
                        Uint16 pc_copy;
                        if(i != 0 && buffer.queue[i-1].type != 1) {
                            clEnqueueReadBuffer(env.queue,mRam,CL_TRUE,0,0x10000 * 0x10 * sizeof(Uint8),u->ram,0,NULL,NULL);
                            clEnqueueReadBuffer(env.queue, mPc, CL_TRUE, 0, sizeof(Uint16), &pc_copy, 0, NULL, NULL);
                            ins = u->ram[pc_copy - 1] & 0xff & 0x40;
                            clEnqueueReadBuffer(env.queue, ins ? mRst : mWst, CL_TRUE, 0, sizeof(Stack),
                                                &s, 0, NULL, NULL);
                        }
                        DEI(buffer.queue[i].a,buffer.queue[i].b)
                        if(i == buffer.ptr - 1){
                            clEnqueueWriteBuffer(env.queue,ins ? mRst : mWst ,CL_TRUE,0,sizeof(Stack),&s,0,NULL,NULL);
                        }
                        break;
                    }
                }

            }
        }
        flag = !flag;
        ++times;
    }


}

int uxn_free(){
    free(mWst);
    free(mRst);
    free(mRam);
    free(mBufferB);
    free(mBufferA);
    free(mPc);
    free(mNum);
    free(mFlag);
    free(ptrA);
    free(ptrB);
    return 1;
}
