
ifndef CC
	CC = gcc
endif

CCFLAGS=-std=c99
LDFLAGS = -framework OpenCL -L./devices

EXECUTABLE = uxncli

all: $(EXECUTABLE)

$(EXECUTABLE): uxncli.o uxn.o system.o gpu.o
	$(CC) $(CFLAGS) -o $(EXECUTABLE) uxncli.o uxn.o system.o gpu.o $(LDFLAGS)

uxn.o: uxn.c
	$(CC) $(CFLAGS) -c uxn.c -o uxn.o

uxncli.o: uxncli.c
	$(CC) $(CFLAGS) -c uxncli.c -o uxncli.o

system.o: devices/system.c devices/system.h
	$(CC) $(CFLAGS) -c devices/system.c -o system.o

gpu.o: devices/gpu.c devices/gpu.h
	$(CC) $(CFLAGS) -c devices/gpu.c -o gpu.o

clean:
	rm -f $(EXECUTABLE) uxncli.o uxn.o system.o gpu.o