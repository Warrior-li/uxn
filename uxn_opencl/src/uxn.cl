#define PEEK2(d) (((d)[0] << 8 | ((d)[1])))
#define POKE2(d, v) { (d)[0] = (v) >> 8; (d)[1] = (v); }

#define T s->dat[s->ptr - 1]
#define N s->dat[s->ptr - 2]
#define L s->dat[s->ptr - 3]
#define H2 PEEK2(s->dat + s->ptr - 3)
#define T2 PEEK2(s->dat + s->ptr - 2)
#define N2 PEEK2(s->dat + s->ptr - 4)
#define L2 PEEK2(s->dat + s->ptr - 6)


#define HALT(c) { *halt = c; return; }
#define SET(mul, add) { if(mul > s->ptr) HALT(1) tmp = (mul & k) + add + s->ptr; if(tmp > 254) HALT(2) s->ptr = tmp; }
#define PUT(o, v) { s->dat[(char)(s->ptr - 1 - (o))] = (v); }
#define PUT2(o, v) { tmp = (v); s->dat[(char)(s->ptr - o - 2)] = tmp >> 8; s->dat[(char)(s->ptr - o - 1)] = tmp; }
#define PUSH(x, v) { z = (x); if(z->ptr > 254) HALT(2) z->dat[z->ptr++] = (v); }
#define PUSH2(x, v) { z = (x); if(z->ptr > 253) HALT(2) tmp = (v); z->dat[z->ptr] = tmp >> 8; z->dat[z->ptr + 1] = tmp; z->ptr += 2; }
#define EXIT {*pca = pc-1; return;}

typedef struct Stack {
    unsigned char dat[255], ptr;
}Stack;



__kernel void uxn_boot(
        __global unsigned char *dev,
        __global Stack *wst,
        __global Stack *rst
        )
{
    unsigned int i = get_global_id(0);
    __global unsigned char *wptr = (__global unsigned char *) wst;
    __global unsigned char *rptr = (__global unsigned char *) rst;
    if( i < 256 ) dev[i] = 0;
    if( i < 256 ) {wptr[i] = 0; rptr[i] = 0;}

}


__kernel void uxn_eval(
        __global unsigned char *ram,
        __global Stack *wst,
        __global Stack *rst,
        __global unsigned char *dev,
        __global unsigned short *pca,
        __global unsigned char *result,
        __global unsigned char *halt,
        __global unsigned short *cord
        )
{
    int t,n,l,k,tmp,ins,opc;
    unsigned short pc =  *pca;
    __global Stack *s,*z;
    if(!pc || dev[0x0f])return;
    for(;;){
        ins = ram[pc++] & 0xff;
        k = ins & 0x80 ? 0xff : 0;
        s = ins & 0x40 ? rst : wst;
        opc = !(ins & 0x1f) ? (0 - (ins >> 5)) & 0xff : ins & 0x3f;
        *result = *result + 1;
        cord[*result] = !!s->dat[s->ptr-1] * PEEK2(ram + pc) + 2;
        switch(opc) {
            /* IMM */
            case 0x00: /* BRK   */ EXIT
            case 0xff: /* JCI   */ pc += !!s->dat[--s->ptr] * PEEK2(ram + pc) + 2; break;
            case 0xfe: /* JMI   */ pc += PEEK2(ram + pc) + 2; break;
            case 0xfd: /* JSI   */ PUSH2(rst, pc + 2) pc += PEEK2(ram + pc) + 2; break;
            case 0xfc: /* LIT   */ PUSH(s, ram[pc++]) break;
            case 0xfb: /* LIT2  */ PUSH2(s, PEEK2(ram + pc)) pc += 2; break;
            case 0xfa: /* LITr  */ PUSH(s, ram[pc++]) break;
            case 0xf9: /* LIT2r */ PUSH2(s, PEEK2(ram + pc)) pc += 2; break;
            /* ALU */
            case 0x01: /* INC  */ t=T;            SET(1, 0) PUT(0, t + 1) break;
            case 0x21:            t=T2;           SET(2, 0) PUT2(0, t + 1) break;
            case 0x02: /* POP  */                 SET(1,-1) break;
            case 0x22:                            SET(2,-2) break;
            case 0x03: /* NIP  */ t=T;            SET(2,-1) PUT(0, t) break;
            case 0x23:            t=T2;           SET(4,-2) PUT2(0, t) break;
            case 0x04: /* SWP  */ t=T;n=N;        SET(2, 0) PUT(0, n) PUT(1, t) break;
            case 0x24:            t=T2;n=N2;      SET(4, 0) PUT2(0, n) PUT2(2, t) break;
            case 0x05: /* ROT  */ t=T;n=N;l=L;    SET(3, 0) PUT(0, l) PUT(1, t) PUT(2, n) break;
            case 0x25:            t=T2;n=N2;l=L2; SET(6, 0) PUT2(0, l) PUT2(2, t) PUT2(4, n) break;
            case 0x06: /* DUP  */ t=T;            SET(1, 1) PUT(0, t) PUT(1, t) break;
            case 0x26:            t=T2;           SET(2, 2) PUT2(0, t) PUT2(2, t) break;
            case 0x07: /* OVR  */ t=T;n=N;        SET(2, 1) PUT(0, n) PUT(1, t) PUT(2, n) break;
            case 0x27:            t=T2;n=N2;      SET(4, 2) PUT2(0, n) PUT2(2, t) PUT2(4, n) break;
            case 0x08: /* EQU  */ t=T;n=N;        SET(2,-1) PUT(0, n == t) break;
            case 0x28:            t=T2;n=N2;      SET(4,-3) PUT(0, n == t) break;
            case 0x09: /* NEQ  */ t=T;n=N;        SET(2,-1) PUT(0, n != t) break;
            case 0x29:            t=T2;n=N2;      SET(4,-3) PUT(0, n != t) break;
            case 0x0a: /* GTH  */ t=T;n=N;        SET(2,-1) PUT(0, n > t) break;
            case 0x2a:            t=T2;n=N2;      SET(4,-3) PUT(0, n > t) break;
            case 0x0b: /* LTH  */ t=T;n=N;        SET(2,-1) PUT(0, n < t) break;
            case 0x2b:            t=T2;n=N2;      SET(4,-3) PUT(0, n < t) break;
            case 0x0c: /* JMP  */ t=T;            SET(1,-1) pc += (signed char)t; break;
            case 0x2c:            t=T2;           SET(2,-2) pc = t; break;
            case 0x0d: /* JCN  */ t=T;n=N;        SET(2,-2) pc += !!n * (signed char)t; break;
            case 0x2d:            t=T2;n=L;       SET(3,-3) if(n) pc = t; break;
            case 0x0e: /* JSR  */ t=T;            SET(1,-1) PUSH2(rst, pc) pc += (signed char)t; break;
            case 0x2e:            t=T2;           SET(2,-2) PUSH2(rst, pc) pc = t; break;
            case 0x0f: /* STH  */ t=T;            SET(1,-1) PUSH((ins & 0x40 ? wst : rst), t) break;
            case 0x2f:            t=T2;           SET(2,-2) PUSH2((ins & 0x40 ? wst : rst), t) break;
	        case 0x10: /* LDZ  */ t=T;            SET(1, 0) PUT(0, ram[t]) break;
            case 0x30:            t=T;            SET(1, 1) PUT2(0, PEEK2(ram + t)) break;
            case 0x11: /* STZ  */ t=T;n=N;        SET(2,-2) ram[t] = n; break;
            case 0x31:            t=T;n=H2;       SET(3,-3) POKE2(ram + t, n) break;
            case 0x12: /* LDR  */ t=T;            SET(1, 0) PUT(0, ram[pc + (signed char)t]) break;
            case 0x32:            t=T;            SET(1, 1) PUT2(0, PEEK2(ram + pc + (signed char)t)) break;
            case 0x13: /* STR  */ t=T;n=N;        SET(2,-2) ram[pc + (signed char)t] = n; break;
            case 0x33:            t=T;n=H2;       SET(3,-3) POKE2(ram + pc + (signed char)t, n) break;
            case 0x14: /* LDA  */ t=T2;           SET(2,-1) PUT(0, ram[t]) break;
            case 0x34:            t=T2;           SET(2, 0) PUT2(0, PEEK2(ram + t)) break;
            case 0x15: /* STA  */ t=T2;n=L;       SET(3,-3) ram[t] = n; break;
            case 0x35:            t=T2;n=N2;      SET(4,-4) POKE2(ram + t, n) break;
            case 0x16: /* DEI  */ EXIT
            case 0x36:            EXIT
            case 0x17: /* DEO  */ EXIT
            case 0x37:            EXIT
            case 0x18: /* ADD  */ t=T;n=N;        SET(2,-1) PUT(0, n + t) break;
            case 0x38:            t=T2;n=N2;      SET(4,-2) PUT2(0, n + t) break;
            case 0x19: /* SUB  */ t=T;n=N;        SET(2,-1) PUT(0, n - t) break;
            case 0x39:            t=T2;n=N2;      SET(4,-2) PUT2(0, n - t) break;
            case 0x1a: /* MUL  */ t=T;n=N;        SET(2,-1) PUT(0, n * t) break;
            case 0x3a:            t=T2;n=N2;      SET(4,-2) PUT2(0, n * t) break;
            case 0x1b: /* DIV  */ t=T;n=N;        SET(2,-1) if(!t) HALT(3) PUT(0, n / t) break;
            case 0x3b:            t=T2;n=N2;      SET(4,-2) if(!t) HALT(3) PUT2(0, n / t) break;
            case 0x1c: /* AND  */ t=T;n=N;        SET(2,-1) PUT(0, n & t) break;
            case 0x3c:            t=T2;n=N2;      SET(4,-2) PUT2(0, n & t) break;
            case 0x1d: /* ORA  */ t=T;n=N;        SET(2,-1) PUT(0, n | t) break;
            case 0x3d:            t=T2;n=N2;      SET(4,-2) PUT2(0, n | t) break;
            case 0x1e: /* EOR  */ t=T;n=N;        SET(2,-1) PUT(0, n ^ t) break;
            case 0x3e:            t=T2;n=N2;      SET(4,-2) PUT2(0, n ^ t) break;
            case 0x1f: /* SFT  */ t=T;n=N;        SET(2,-1) PUT(0, n >> (t & 0xf) << (t >> 4)) break;
            case 0x3f:            t=T;n=H2;       SET(3,-1) PUT2(0, n >> (t & 0xf) << (t >> 4)) break;
        }
        //if(*result == 33) {*pca = pc; return;}
    }
}