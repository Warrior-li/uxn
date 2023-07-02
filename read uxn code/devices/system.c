//
// Created by Warrior on 28/06/2023.
//

#include <stdio.h>
#include <stdlib.h>

#include "../uxn.h"
#include "system.h"

static const char *errors[] = {
        "underflow",
        "overflow",
        "division by zero"
};

static void
system_print(Stack *s, char *name){
    Uint8 i;
    fprintf(stderr,"<%s>",name);
    for(i = 0; i < s->ptr; ++i){
        fprintf(stderr,"%02x",s->dat[i]);
    }
    if(!i) fprintf(stderr, " empty");
    fprintf(stderr, "\n");
}

static void
system_cmd(Uint8 *ram, Uint16 addr){
    if(ram[addr] == 0x1){
        Uint16 i,length = PEEK2(ram + addr + 1);
    }
}