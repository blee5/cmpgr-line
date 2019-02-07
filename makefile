OBJECTS= main.o draw.o imageio.o
CFLAGS= -Wall
CC= gcc

run: main
	./main

main: $(OBJECTS)
	$(CC) -o main $(OBJECTS)

main.o: main.c imageio.h draw.h image.h
	$(CC) -c main.c

draw.o: draw.c draw.h imageio.h image.h
	$(CC) $(CFLAGS) -c draw.c

imageio.o: imageio.c imageio.h image.h
	$(CC) $(CFLAGS) -c imageio.c

clean:
	rm *.o
