#include <stdio.h>
/* ./ex10_hello-world.tal */
static char mem[64*1024] =  "Hello, World!";
// static char hello_world[] = "Hello, World!";
static char hello_world_=0;
static short rr0_=16;
// static char* rr0=&rr0_;
int main() {
    
    short r1_=18;
    short r2_=20;
    short r3_=22;
    short r4_=24;
    r4_ = 2400;
    // mem[r2_] = r4_>>8; mem[r2_+1] = r4_&0xFF;
    // r3_ = (mem[r2_]<<8)+mem[r2_+1];
    // printf("%d<>%d\n",r3_,r4_);
    *((short*)(&mem[r2_])) = r4_;
    r3_ = *((short*)(&mem[r2_]));
    printf("%d<>%d\n",r3_,r4_);

    // char* r1=&r1_;
    // char* r2=&r2_;
    // char* r3=&r3_;
    // char* r4=&r4_;
    /* rr0 is a label for an address, and so is hello_world.
    So what we really have is
    */
    rr0_=hello_world_;
    //rr0_ = hello_world;  //  ;hello_world ;rr0 STA2
    on_reset_while: 
        r1_ = mem[rr0_];  //;rr0 LDA2 LDA ,&r1 STR
        printf("%c",r1_); // ,&r1 LDR #18 DEO  
        r2_ = rr0_+1; // ;rr0 LDA2 INC2 ,&r2 STR2
        // // ,&r5 LDR2 ,&r6 LDR2 ADD2 ,&r7 STR2
        // // *r7 = *r5 + *r6 but how do I distinguish between this and pointer arithmetic?
        // printf("<%c>",r2_);
        // r3 =  r2; // ,&r2 LDR2 LDA ,&r3 STR
        r3_ = mem[r2_];
        // *r4 = (*r3)!=0; // ,&r3 LDR #00 NEQ ,&r4 STR
        r4_ = r3_!=0;
        // rr0 = r2; // ,&r2 LDR2 ;rr0 STA2
        rr0_ = r2_;
        // if (*r4) { goto on_reset_while; }
        if (r4_) { goto on_reset_while; }

                             
    return 0;
    }

/*
- we need a global memory counter for all labels etc. Although in practice I think we can count non-function labels only. So it might be useful to store the allocated or used size in the block. I am inclined to store the zero-page memory separately. 
- we need syntax pattern matching :
    a #18 DEO => printf("%c",a);
    a INC2 => a+1
    a b NEQ => a != b
    a b JCN => if (a) { goto b ; }
    a JMP => goto a;
    a b f JSR2 r => r = f(a,b)
    a b BINOP => a `binop` b 

*/


