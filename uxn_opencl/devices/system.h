//
// Created by Warrior on 22/07/2023.
//

#define RAM_PAGES 0x10

int system_error(char *msg, const char *err);
int system_load(Uxn *u, char *filename,cl_context context, cl_command_queue queue, cl_device_id deviceId, cl_mem memU);