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

$(TARGET): main.o functions.o
	$(CC) $(CFLAGS) $(RFLAGS) -o $(TARGET) main.o functions.o -lm -lcrypt -pthread

main.o: main.c
	$(CC) $(CFLAGS) -c -o main.o main.c

functions.o: functions.c
	$(CC) $(CFLAGS) -c -o functions.o functions.c

clean:
	rm *.o $(TARGET)

remake: clean $(TARGET)
