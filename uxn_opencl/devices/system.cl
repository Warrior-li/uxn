typedef struct Stack {
    char dat[255], ptr;
}Stack;

typedef struct Uxn {
    char *ram, dev[256];
    Stack wst, rst;
}Uxn;

__kernel void system_load(
        __global Uxn* u,
        __global char* ram,
        )
{
    unsigned int i = get_global_id(0);
    u->ram[i] = ram[i];
}