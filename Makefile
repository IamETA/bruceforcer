# set compiler: g++ for C++
CC = gcc

#Flags
CFLAGS = -g -Wall -Wextra
DFLAGS = -O3
RFLAGS = -O3

#OBJ folder
OBJDIR = obj

#name of files
TARGET = bruceforce

$(TARGET): main.o functions.o threads.o engines.o server.o networker.o
	$(CC) $(CFLAGS) $(RFLAGS) -o $(TARGET) main.o functions.o threads.o engines.o server.o networker.o -lm -lcrypt -pthread

main.o: main.c
	$(CC) $(CFLAGS) -c -o main.o main.c

functions.o: functions.c
	$(CC) $(CFLAGS) -c -o functions.o functions.c

threads.o: threads.c
	$(CC) $(CFLAGS) -c -o threads.o threads.c

server.o: server.c
	$(CC) $(CFLAGS) -c -o server.o server.c

engines.o: engines.c
	$(CC) $(CFLAGS) -c -o engines.o engines.c

networker.o: networker.c
	$(CC) $(CFLAGS) -c -o networker.o networker.c

clean:
	rm *.o $(TARGET)

remake: clean $(TARGET)
