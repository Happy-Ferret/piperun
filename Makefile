#
# piperun - Run ELF binary code read from standard input.
#
# AUTHOR: Joey Pabalinas <alyptik@protonmail.com>
# See LICENSE file for copyright and license details.

CC ?= gcc
LD ?= $(CC)
PREFIX ?= $(DESTDIR)/usr/local
TARGET_ARCH ?= -march=x86-64 -mtune=generic
CFLAGS = -O2 -pipe -MMD -I. -fPIC -fstack-protector-strong -Wall -Wextra -std=c11 -pedantic-errors -D_GNU_SOURCE
LDFLAGS = -Wl,-O1,--sort-common,--as-needed,-z,relro -Wl,-z,now

SRC = $(wildcard *.c)
TSRC = $(wildcard t/*.c)
OBJ = $(patsubst %.c, %.o, $(wildcard *.c))
TOBJ = $(patsubst %, %.o, $(TESTS))
HDR = $(wildcard *.h) $(wildcard t/*.h)
TESTS = $(patsubst %.c, %, $(TSRC))

TARGET = piperun

%:
	$(CC) $(LDFLAGS) $(TARGET_ARCH) $(filter %.o, $^) -o $@

%.o:
	$(CC) $(CFLAGS) $(TARGET_ARCH) -c $(filter %.c, $^) -o $@

all: $(TARGET) tests

$(TARGET): $(OBJ)

$(OBJ): %.o: %.c $(HDR)

check test: $(TARGET) tests
	@printf "\n%s\n" "piped output:"
	$(CC) $(LDFLAGS) $(TARGET_ARCH) $(TSRC) -o /dev/stdout | ./$(TARGET)
	@printf "\n%s\n" "non-piped output:"
	$(TESTS)

tests: $(TESTS)

$(TESTS): %: %.o

$(TOBJ): %.o: %.c $(HDR)

install: $(TARGET)
	@printf "%s\n" "installing:"
	@mkdir -pv $(PREFIX)/bin
	install -c $(TARGET) $(PREFIX)/bin

uninstall:
	@rm -fv $(PREFIX)/bin/$(TARGET)

dist: clean
	@printf "%s\n" "creating dist tarball:"
	@mkdir -pv $(TARGET)
	@cp -Rv LICENSE Makefile README.md $(HDR) $(SRC) $(TSRC) $(TARGET)
	tar -czf $(TARGET).tar.gz $(TARGET)
	@rm -rfv $(TARGET)

clean:
	@printf "%s\n" "cleaning:"
	@rm -fv $(TARGET) $(OBJ) $(TOBJ) $(TESTS) $(TARGET).tar.gz $(wildcard *.d)

-include $(wildcard *.d) $(wildcard t/*.d)
.PHONY: all clean install uninstall dist check test tests
