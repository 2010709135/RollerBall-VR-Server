CC = gcc
CFLAGS = -W -Wall
TARGET = server
OBJECTS = last_server.o RoomInfo.o UserInfo.o

all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

