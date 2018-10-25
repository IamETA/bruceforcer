# set compiler: g++ for C++
CC = gcc

#Flags
CFLAGS = -g -Wall -Wextra
DFLAGS = -O0
RFLAGS = -O0

#OBJ folder
OBJDIR = obj

#name of files
TARGET = bruceforce

$(TARGET): main.o
	$(CC) $(CFLAGS) $(RFLAGS) -o $(TARGET) main.o -lm -lcrypt

main.o: main.c
	$(CC) $(CFLAGS) -c -o main.o main.c

clean:
	rm *.o $(TARGET)

remake: clean $(TARGET)
