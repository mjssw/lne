# Makefile for LNE

.PHONY : all clean install uninstall

all:
	cd lne && $(MAKE) $@
	cd test && $(MAKE) $@

clean:
	cd lne && $(MAKE) $@
	cd test && $(MAKE) $@

install:
	cd lne && $(MAKE) $@

uninstall:
	cd lne && $(MAKE) $@


