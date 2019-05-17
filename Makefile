#Makefile for Crystalfontz UNIX examples

CC ?= gcc
STRIP ?= strip
LIBS = 
CFLAGS = -g -O -Iinclude -Wall
LDFLAGS = -g

LIBSRC = include/serial.c
LIBOBJ = $(LIBSRC:%.c=%.o)

all: serial_send

serial_send: src/serial_send.o $(LIBOBJ)
	$(CC) $(LDFLAGS) $(LIBOBJ) src/serial_send.o -o serial_send
	$(STRIP) serial_send

clean:
	rm -f $(LIBOBJ) src/serial_send.o serial_send
