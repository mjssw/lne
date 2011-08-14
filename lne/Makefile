# Makefile for LNE

.PHONY : all clean install uninstall

# generate config-os.h
OSMACRO := $(shell sh -c './osmacro.sh')
VERSION := $(shell sh -c './version.sh')

CC = g++
LN = ln -sf
CFLAGS += -Wall -fPIC
LDFLAGS += -shared -pthread
PREFIX = /usr
SONAME = liblne.so
TARGET = $(SONAME).$(VERSION)
OBJS = $(patsubst %.cpp,%.o,$(wildcard *.cpp))

ifeq ($(RELEASE),y)
	CFLAGS += -O3 -DNDEBUG
	LDFLAGS += -s
else
	CFLAGS += -g -ggdb -rdynamic
endif

all: $(TARGET)

%.o: %.cpp %.inl %.h
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJS) -Wl,--soname=$(SONAME)
	$(LN) $(TARGET) $(SONAME)

clean:
	rm -fr $(SONAME) $(TARGET) *.o config-os.h

install: all
	mkdir -p $(PREFIX)/lib
	mkdir -p $(PREFIX)/include/lne
	cp -p $(TARGET) $(PREFIX)/lib
	cp -p *.h *.inl $(PREFIX)/include/lne
	$(LN) $(PREFIX)/lib/$(TARGET) $(PREFIX)/lib/$(SONAME)

uninstall:
	rm -fr $(PREFIX)/include/lne
	rm -f $(PREFIX)/lib/$(SONAME)
	rm -f $(PREFIX)/lib/$(TARGET)

