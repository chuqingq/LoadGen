CC      := g++
CFLAGS  := -DNDEBUG -g -Wall -Werror -Wl,-export-dynamic
LDFLAGS :=  -Wall -Werror -Wl,-export-dynamic
LIBS    := -Llib/libuv/lib -luv -Llib/libjson/lib -ljson -ldl -lpthread -lrt

SRC  := ls_master.c ls_session.c ls_task_script.c ls_task_var.c main.c ls_logger.c ls_plugin.c ls_task_callmodel.c ls_task_setting.c ls_worker.c
BIN  := loadgen

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

