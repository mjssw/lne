# Makefile for LNE

.PHONY : all clean

ifeq ($(RELEASE),y)
	CFLAGS += -O3
	LDFLAGS += -s
else
	CFLAGS += -g -ggdb -rdynamic
endif

CC = g++
CFLAGS += -Wall -I../
LDFLAGS += -pthread -L../lne -llne
TARGET = test

OBJS = $(patsubst %.cpp,%.o,$(wildcard *.cpp))

all: $(TARGET)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJS)

clean:
	rm -fr $(TARGET) *.o

