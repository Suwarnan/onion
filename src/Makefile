include ../Makefile.config

CC=gcc
CFLAGS=-Wall $(JUDY_INC)
LDFLAGS=-lJudy $(JUDY_LIB)

OBJS=version.o buzhash.o
TARGETS=hashgen hashdup onion
all: $(TARGETS)

%.o: %.h
$(TARGETS): $(OBJS)

install: $(TARGETS)
	mkdir -p $(INSTALL_BIN) && install -m 755 $(TARGETS) $(INSTALL_BIN)

clean:
	rm -f $(OBJS) $(TARGETS)
