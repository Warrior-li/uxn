#define PEEK2(d) ((d)[0] << 8 | (d)[1])


#define HALT(c) { return uxn_halt(u, ins, (c), pc - 1); }

#define PUSH2(x, v) { z = (x); if(z->ptr > 253) HALT(2) tmp = (v); z->dat[z->ptr] = tmp >> 8; z->dat[z->ptr + 1] = tmp; z->ptr += 2; }


typedef struct Stack {
    char dat[255], ptr;
}Stack;

typedef struct Uxn {
    char *ram, dev[256];
    Stack wst, rst;
}Uxn;

__kernel void uxn_boot(
        __global Uxn* u,
        __global char* ram
        )
{
    unsigned int i = get_global_id(0);
    __global char *cptr = (__global char*)u;
    if( i < sizeof(Uxn) ) cptr[i] = 0;
    u->ram[i] = ram[i];
}


__kernel void uxn_eval(
        __global Uxn* u,
        __global short *pc
        )
{
    int t, n, l, k, tmp, opc, ins;
    char *ram = u->ram;
    Stack *s, *z;
    if(!pc|| u->dev[0x0f]) return;
    for(;;){
        ins = ram[pc++] & 0xff;
        k = ins & 0x80 ? 0xff : 0;
        s = ins & 0x40 ? &u->rst : &u->wst;
        opc = !(ins & 0x1f) ? (0 - (ins >> 5)) & 0xff : ins & 0x3f;
        if(opc == 0x00) return;
        else if(opc == 0xff) pc += s->dat[--s->ptr] * PEEK2(ram + pc) + 2 ;
        else if(opc == 0xfe) pc += PEEK2(ram + pc) + 2;
        else if(opc == 0xfd) PUSH2(&u->rst, pc + 2) pc += PEEK2(ram + pc) + 2;
    }
}