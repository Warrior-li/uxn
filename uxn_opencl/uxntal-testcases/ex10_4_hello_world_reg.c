#include <stdio.h>
/* ./ex10_hello-world.tal */
static char hello_world[] = "Hello, World!";
static short rr0_;
static char* rr0=&rr0_;
int main() {
    short r1_;
    short r2_;
    short r3_;
    short r4_;
    char* r1=&r1_;
    char* r2=&r2_;
    char* r3=&r3_;
    char* r4=&r4_;
    rr0 = hello_world;  //  ;hello_world ;rr0 STA2
    on_reset_while: 
        r1 = rr0;  //;rr0 LDA2 LDA ,&r1 STR
        printf("%c",*r1);   
        r2 = rr0+1; // ;rr0 LDA2 INC2 ,&r2 STR2
        // printf("%c",*r2);
        r3 =  r2; // ,&r2 LDR2 LDA ,&r3 STR
        *r4 = (*r3)!=0; // ,&r3 LDR #00 NEQ ,&r4 STR
        rr0 = r2; // ,&r2 LDR2 ;rr0 STA2
        if (*r4==1) { goto on_reset_while; }
                             
    return 0;
    }
