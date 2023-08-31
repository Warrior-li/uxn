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
        printf("%c",*r1); // ,&r1 LDR #18 DEO  
        r2 = rr0+1; // ;rr0 LDA2 INC2 ,&r2 STR2
        // printf("%c",*r2);
        r3 =  r2; // ,&r2 LDR2 LDA ,&r3 STR
        *r4 = (*r3)!=0; // ,&r3 LDR #00 NEQ ,&r4 STR
        rr0 = r2; // ,&r2 LDR2 ;rr0 STA2
        if (*r4) { goto on_reset_while; }
                             
    return 0;
    }

/*
So a working translation rule is: 
- a GlobRegSet is a straight assignment
- a RegOperation results in *s on both sides unless token_.op.opcode.name ~~ /LDA/;
- we need syntax pattern matching :
    a #18 DEO => printf("%c",a);
    a INC2 => a+1
    a b NEQ => a != b
    a b JCN => if (a) { goto b ; }
    a JMP => goto a;
    a b f JSR2 r => r = f(a,b)
    a b BINOP => a `binop` b 

*/