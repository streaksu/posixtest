# GNUmakefile: Makefile of util-vinix.
# Code is governed by the GPL-2.0 license.
# Copyright (C) 2021-2022 The Vinix authors.

ifeq ($(origin CC), default)
	CC := cc
endif
CFLAGS  ?= -O2 -pipe -g
PREFIX  ?= /usr/local

# Targets and their rules.
override TARGET := socket_test

.PHONY: all clean install

all: $(TARGET)

$(TARGET):
	$(CC) $(CFLAGS) main.c -o $(TARGET)

clean:
	rm -rf $(TARGET)

install:
	install -d "$(DESTDIR)$(PREFIX)/bin"
	install -s $(TARGET) "$(DESTDIR)$(PREFIX)/bin/"
