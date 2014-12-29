CC      := g++
CFLAGS  := -DNDEBUG -g -Wall -Werror -fPIC -I../../ -I../../lib -O2
LDFLAGS := -Wall -Werror -fPIC -shared -O2
LIBS    := 

SRC  := ls_plugin_system.c
BIN  := ls_plugin_system.so

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

