# Makefile for LNE

.PHONY : all clean install uninstall

# generate config-os.h
OSMACRO := $(shell sh -c './osmacro.sh')
VERSION := $(shell sh -c './version.sh')
MAJORVN := $(firstword $(subst ., ,$(VERSION)))

CC = g++
LN = ln -sf
CFLAGS += -Wall -fPIC
CXXFLAGS +=
LDFLAGS += -shared -pthread
PREFIX = /usr
LNNAME = liblne.so
SONAME  = $(LNNAME).$(MAJORVN)
TARGET  = $(LNNAME).$(VERSION)
OBJS = $(patsubst %.cpp,%.o,$(wildcard *.cpp))

ifeq ($(RELEASE),y)
	CFLAGS += -O3 -DNDEBUG
	CXXFLAGS += -fno-rtti -fvisibility-inlines-hidden
	LDFLAGS += -s
else
	CFLAGS += -g -ggdb -rdynamic
endif

all: $(TARGET)

%.o: %.cpp %.h
	$(CC) $(CFLAGS) $(CXXFLAGS) -c $< -o $@
%.o: %.cpp
	$(CC) $(CFLAGS) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJS) -Wl,--soname=$(SONAME)
	$(LN) $(TARGET) $(SONAME)
	$(LN) $(SONAME) $(LNNAME)

clean:
	rm -fr $(LNNAME) $(SONAME) $(TARGET) *.o config-os.h

install: all
	mkdir -p $(PREFIX)/lib
	mkdir -p $(PREFIX)/include/lne
	cp -p $(TARGET) $(PREFIX)/lib
	cp -p *.h $(PREFIX)/include/lne
	$(LN) $(PREFIX)/lib/$(TARGET) $(PREFIX)/lib/$(SONAME)
	$(LN) $(PREFIX)/lib/$(SONAME) $(PREFIX)/lib/$(LNNAME)

uninstall:
	rm -fr $(PREFIX)/include/lne
	rm -f $(PREFIX)/lib/$(LNNAME)
	rm -f $(PREFIX)/lib/$(SONAME)
	rm -f $(PREFIX)/lib/$(TARGET)

