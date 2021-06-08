
CC=emcc
CFLAGS=-O2 -Isrc/libdivsufsort/include -Isrc
OBJDIR=obj
LDFLAGS= -sINITIAL_MEMORY=268435456 -sEXPORTED_RUNTIME_METHODS='["cwrap"]' -sEXPORTED_FUNCTIONS=_pack,_unpack,_malloc,_free

$(OBJDIR)/%.o: src/../%.c
	@mkdir -p '$(@D)'
	$(CC) $(CFLAGS) -c $< -o $@

APP := apultra.js

OBJS += $(OBJDIR)/src/apultra.o
OBJS += $(OBJDIR)/src/expand.o
OBJS += $(OBJDIR)/src/matchfinder.o
OBJS += $(OBJDIR)/src/shrink.o
OBJS += $(OBJDIR)/src/libdivsufsort/lib/divsufsort.o
OBJS += $(OBJDIR)/src/libdivsufsort/lib/divsufsort_utils.o
OBJS += $(OBJDIR)/src/libdivsufsort/lib/sssort.o
OBJS += $(OBJDIR)/src/libdivsufsort/lib/trsort.o

all: $(APP)

$(APP): $(OBJS)
	$(CC) $^ $(LDFLAGS) -o $(APP)

clean:
	@rm -rf $(APP) $(OBJDIR) *.wasm
