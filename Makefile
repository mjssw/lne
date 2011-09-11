# Makefile for LNE

.PHONY : all clean install uninstall

all:
	cd lne && $(MAKE) $@
	cd test && $(MAKE) $@
	cd apps && $(MAKE) $@

clean:
	cd lne && $(MAKE) $@
	cd test && $(MAKE) $@
	cd apps && $(MAKE) $@

install:
	cd lne && $(MAKE) $@
	cd apps && $(MAKE) $@

uninstall:
	cd lne && $(MAKE) $@
	cd apps && $(MAKE) $@

