# Makefile for LNE

.PHONY : all clean install uninstall

CC = g++
CFLAGS += -Wall -I../../
LDFLAGS += -pthread -L../../lne -llne
PREFIX = /usr
TARGET = pproxy
OBJS = $(patsubst %.cpp,%.o,$(wildcard *.cpp))

ifeq ($(RELEASE),y)
	CFLAGS += -O3
	LDFLAGS += -s
else
	CFLAGS += -g -ggdb -rdynamic
endif

all: $(TARGET)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJS)

clean:
	rm -fr $(TARGET) *.o

install: all
	mkdir -p $(PREFIX)/bin
	cp -p $(TARGET) $(PREFIX)/bin

uninstall:
	rm -f $(PREFIX)/bin/$(TARGET)
