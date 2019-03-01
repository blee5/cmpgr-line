OBJECTS = main.o draw.o imageio.o matrix.o parser.o
CFLAGS = -Wall -g
LDFLAGS = -lm
CC = gcc

run: main
	@./main script

main: $(OBJECTS)
	$(CC) $(CFLAGS) -o main $(OBJECTS) $(LDFLAGS)

main.o: main.c imageio.h draw.h image.h matrix.h parser.h
	$(CC) $(CFLAGS) -c main.c

draw.o: draw.c draw.h image.h
	$(CC) $(CFLAGS) -c draw.c

imageio.o: imageio.c imageio.h image.h
	$(CC) $(CFLAGS) -c imageio.c

matrix.o: matrix.c matrix.h
	$(CC) $(CFLAGS) -c matrix.c

parser.o: parser.c parser.h
	$(CC) $(CFLAGS) -c parser.c

clean:
	rm *.o
