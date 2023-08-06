ifndef CC
	CC = gcc
endif

CFLAGS= -g -Wno-typedef-redefinition -D_C99_SOURCE
LDFLAGS = -framework OpenCL -L./devices

EXECUTABLE = uxncli

all: $(EXECUTABLE)

$(EXECUTABLE): uxncli.o uxn.o system.o gpu.o file.o datetime.o
	$(CC) $(CFLAGS) -o $(EXECUTABLE) uxncli.o uxn.o system.o gpu.o file.o datetime.o $(LDFLAGS)

uxn.o: uxn.c
	$(CC) $(CFLAGS) -c uxn.c -o uxn.o

uxncli.o: uxncli.c
	$(CC) $(CFLAGS) -c uxncli.c -o uxncli.o

system.o: devices/system.c devices/system.h
	$(CC) $(CFLAGS) -c devices/system.c -o system.o

gpu.o: devices/gpu.c devices/gpu.h
	$(CC) $(CFLAGS) -c devices/gpu.c -o gpu.o

file.o: devices/file.c devices/file.h
	$(CC) $(CFLAGS) -c devices/file.c -o file.o

datetime.o: devices/datetime.c devices/datetime.h
	$(CC) $(CFLAGS) -c devices/datetime.c -o datetime.o

clean:
	rm -f $(EXECUTABLE) uxncli.o uxn.o system.o gpu.o datetime.o file.o