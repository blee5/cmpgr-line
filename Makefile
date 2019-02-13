OBJECTS = main.o draw.o imageio.o matrix.o
CFLAGS = -Wall -g
CC = gcc

run: main
	@./main

main: $(OBJECTS)
	$(CC) $(CFLAGS) -o main $(OBJECTS)

main.o: main.c imageio.h draw.h image.h matrix.h 
	$(CC) $(CFLAGS) -c main.c

draw.o: draw.c draw.h image.h
	$(CC) $(CFLAGS) -c draw.c

imageio.o: imageio.c imageio.h image.h
	$(CC) $(CFLAGS) -c imageio.c

matrix.o: matrix.c matrix.h
	$(CC) $(CFLAGS) -c matrix.c

clean:
	rm *.o
