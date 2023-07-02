//
// Created by Warrior on 28/06/2023.
//

typedef unsigned char Uint8;
typedef signed char Sint8;
typedef unsigned short Uint16;
typedef signed short Sint16;
typedef unsigned int Uint32;


typedef struct {
    Uint8 dat[255], ptr;
}Stack;

typedef struct Uxn {
    Uint8 *ram, dev[256];
    /* 2 stack in tutorial */
    Stack wst, rst;
    Uint8 (*dei)(struct Uxn *u, Uint8 addr);
    void (*deo)(struct Uxn *u, Uint8 addr);
}Uxn;


/* some function like tutorial */
extern Uint8 uxn_dei(Uxn *u, Uint8 addr);
extern void uxn_deo(Uxn *u, Uint8 addr);
extern int uxn_halt(Uxn *u, Uint8 instr, Uint8 err, Uint16);
extern Uint16 dei_mask[];
extern Uint16 deo_mask[];


/* start */

int uxn_boot(Uxn *u, Uint8 *ram);
int uxn_eval(Uxn *u, Uint16 pc);