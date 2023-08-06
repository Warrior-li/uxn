//
// Created by Warrior on 22/07/2023.
//

#define RAM_PAGES 0x10

#define CONSOLE_STD 0x1
#define CONSOLE_ARG 0x2
#define CONSOLE_EOA 0x3
#define CONSOLE_END 0x4

int system_load(Uxn *u, char *filename);
void system_inspect(Uxn *u);
int system_error(char *msg, const char *err);
void system_deo(Uxn *u, Uint8 *d, Uint8 port);
int console_input(Uxn *u, char c, int type);
void console_deo(Uint8 *d, Uint8 port);