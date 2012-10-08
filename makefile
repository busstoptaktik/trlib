BUILDDIR = ../BUILD/trlib
CC = gcc
CFLAGS = -W -Wall -pedantic -fPIC -O -pg -g -I./TR_INC -I./TR_INC/DEPRECATED -DTHREAD_SAFE -D_ROUT
#-pthread
LDFLAGS = -I./TR_INC -shared  -DTHREAD_SAFE -pg -g
#-pthread
 
EXENAME = main

SRCS   =  $(wildcard TR_SRC/*.c)
LORDSRC = $(wildcard TR_SRC/LORD/*c)
DEPSRC =  $(wildcard TR_SRC/DEPRECATED/*.c)
APISRC =  $(wildcard TR_SRC/API/*.c)
JNISRC =  $(wildcard java/*.c)
#ALLSRC =  $(SRCS) $(APISRC) $(JNISRC) $(DEPSRC)
ALLSRC =  $(SRCS) $(LORDSRC) $(DEPSRC) $(APISRC)

OBJS   =  $(patsubst TR_SRC/%.c,     $(BUILDDIR)/%.o,  $(SRCS))
LORDOBJ = $(patsubst TR_SRC/LORD/%.c,     $(BUILDDIR)/%.o,  $(LORDSRC))
APIOBJ =  $(patsubst TR_SRC/API/%.c, $(BUILDDIR)/%.o,  $(APISRC))
DEPOBJ =  $(patsubst TR_SRC/DEPRECATED/%.c, $(BUILDDIR)/%.o,  $(DEPSRC))
JNIOBJ =  $(patsubst java/%.c,       $(BUILDDIR)/%.o,  $(JNISRC))
#ALLOBJ =  $(OBJS) $(APIOBJ) $(JNIOBJ) $(DEPOBJ)
ALLOBJ =  $(OBJS) $(LORDOBJ) $(DEPOBJ) $(APIOBJ)

.PHONY: pre

all: lib so

lib: pre $(ALLOBJ)

so:
	$(CC) $(LDFLAGS) -o $(BUILDDIR)/libtr.so $(BUILDDIR)/*.o

pre:
	-rm ../BUILD/trlib/compile.err

#../BUILD/trlib/%.o: TR_SRC/%.c
$(BUILDDIR)/%.o: TR_SRC/%.c
	@mkdir -p $(dir $@)
	$(CC) -c -o $@ $< $(CFLAGS) 2>>../BUILD/trlib/compile.err
$(BUILDDIR)/%.o: TR_SRC/LORD/%.c
	@mkdir -p $(dir $@)
	$(CC) -c -o $@ $< $(CFLAGS) 2>>../BUILD/trlib/compile.err
$(BUILDDIR)/%.o: TR_SRC/API/%.c
	@mkdir -p $(dir $@)
	$(CC) -c -o $@ $< $(CFLAGS) 2>>../BUILD/trlib/compile.err
$(BUILDDIR)/%.o: TR_SRC/DEPRECATED/%.c
	@mkdir -p $(dir $@)
	$(CC) -c -o $@ $< $(CFLAGS) 2>>../BUILD/trlib/compile.err
$(BUILDDIR)/%.o: java/%.c
	@mkdir -p $(dir $@)
	$(CC) -c -o $@ $< $(CFLAGS) 2>>../BUILD/trlib/compile.err





$(EXENAME): $(OBJS)
	$(CC) -o $@ $< $(LDFLAGS) 

clean:
	rm -rf $(EXENAME) ../BUILD/trlib/
