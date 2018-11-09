CC = gcc
CFLAGS = -g -Wextra -Wall -ansi
DODATNO = -lXext -lX11 -lXxf86vm -lm -lXi 
LDFLAGS = -lGL -lGLU -lglut $(DODATNO) 

flosXONIX.o : main.c
	$(CC) -o $@ $^ $(LDFLAGS) $(CFLAGS)    

.PHONY: 
	brisi all

brisi:
	 rm -f flosXONIX.o 

all: flosXONIX.o


