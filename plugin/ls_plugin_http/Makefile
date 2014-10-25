CC      := g++
CFLAGS  := -DNDEBUG -g -Wall -Werror -I../../ -I../../lib
LDFLAGS := -Wall -Werror -fPIC -shared
LIBS    := -L../../lib/libuv/lib -luv

SRC  := ls_plugin_http.c
BIN  := ls_plugin_http.so

ODIR := obj
OBJ  := $(patsubst %.c,$(ODIR)/%.o,$(SRC))

all: $(BIN)

clean:
	$(RM) $(BIN) obj/*

$(BIN): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

$(OBJ): Makefile | $(ODIR)

$(ODIR):
	@mkdir $@

$(ODIR)/%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: all clean
.SUFFIXES:
.SUFFIXES: .c .o

## vpath %.c src
## vpath %.h src

