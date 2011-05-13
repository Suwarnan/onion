include ../Makefile.config

CC=gcc
CFLAGS=-Wall -O3 $(JUDY_INC)
LIBS=$(JUDY_LIB) -lJudy 

OBJS=version.o buzhash.o
TARGETS=hashgen hashdup onion
all: $(TARGETS)

$(TARGETS): $(OBJS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< $(LIBS)

%: %.c
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

install: $(TARGETS)
	mkdir -p $(INSTALL_BIN) && install -m 755 $(TARGETS) $(INSTALL_BIN)

clean:
	rm -f $(OBJS) $(TARGETS)
