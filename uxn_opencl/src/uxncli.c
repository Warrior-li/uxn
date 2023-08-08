#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#include "uxn.h"
#include "devices/system.h"
#include "devices/gpu.h"
#include "devices/datetime.h"
#include "devices/file.h"



Uint16 deo_mask[] = {0xc028, 0x0300, 0xc028, 0x8000, 0x8000, 0x8000, 0x8000, 0x0000, 0x0000, 0x0000, 0xa260, 0xa260, 0x0000, 0x0000, 0x0000, 0x0000};
Uint16 dei_mask[] = {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x07ff, 0x0000, 0x0000, 0x0000};


Uint8
uxn_dei(Uxn *u, Uint8 addr)
{
    switch(addr & 0xf0) {
        case 0xc0: return datetime_dei(u, addr);
    }
    return u->dev[addr];
}

void
uxn_deo(Uxn *u, Uint8 addr)
{
    Uint8 p = addr & 0x0f, d = addr & 0xf0;
    switch(d) {
        case 0x00: system_deo(u, &u->dev[d], p); break;
        case 0x10: console_deo(&u->dev[d], p); break;
        case 0xa0: file_deo(0, u->ram, &u->dev[d], p); break;
        case 0xb0: file_deo(1, u->ram, &u->dev[d], p); break;
    }
}


int
main(int argc, char **argv){

    clock_t start_time, end_time;
    double run_time;

    start_time = clock();

    Uxn u;
    int i = 1;
    if(i == argc) return system_error("usage", "uxncli file.rom [args..]");



    if(!opencl_init(&env))
        return system_error("opencl_init","OpenCL init failed");



    if(!uxn_boot(&u,(Uint8 *)calloc(0x10000 * RAM_PAGES, sizeof(Uint8))))
        return system_error("Boot", "Failed");


    if(!system_load(&u,
                    argv[i++]))
        return system_error("Load", "Failed");

    u.dev[0x17] = argc - i;
    read_uxn_from_gpu(&env,&u,&uxn_m);



    if(uxn_eval(&u,
                PAGE_PROGRAM)){
        for(; i < argc; i++) {
            char *p = argv[i];
            while(*p) console_input(&u, *p++, CONSOLE_ARG);
            console_input(&u, '\n', i == argc - 1 ? CONSOLE_END : CONSOLE_EOA);
        }
        while(!u.dev[0x0f]) {
            int c = fgetc(stdin);
            if(c != EOF) console_input(&u, (Uint8)c, CONSOLE_STD);
        }
    }

    end_time = clock();


    run_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("运行时间: %f 秒\n", run_time);

    return u.dev[0x0f] & 0x7f;

}

