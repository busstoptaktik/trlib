BUILDDIR = ../BUILD/trlib
CC = gcc
CFLAGS = -W -Wall -pedantic -fPIC -O -g -I./TR_INC -DTHREAD_SAFE -D_ROUT
#-pthread
LDFLAGS = -I./TR_INC -shared  -DTHREAD_SAFE 
#-pthread
 
EXENAME = main

SRCS   =  $(wildcard TR_SRC/*.c)
APISRC =  $(wildcard TR_SRC/API/*.c)
JNISRC =  $(wildcard java/*.c)
#ALLSRC =  $(SRCS) $(APISRC) $(JNISRC)
ALLSRC =  $(SRCS) $(APISRC)

OBJS   =  $(patsubst TR_SRC/%.c,     $(BUILDDIR)/%.o,  $(SRCS))
APIOBJ =  $(patsubst TR_SRC/API/%.c, $(BUILDDIR)/%.o,  $(APISRC))
JNIOBJ =  $(patsubst java/%.c,       $(BUILDDIR)/%.o,  $(JNISRC))
#ALLOBJ =  $(OBJS) $(APIOBJ) $(JNIOBJ)
ALLOBJ =  $(OBJS) $(APIOBJ)

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
$(BUILDDIR)/%.o: TR_SRC/API/%.c
	@mkdir -p $(dir $@)
	$(CC) -c -o $@ $< $(CFLAGS) 2>>../BUILD/trlib/compile.err
$(BUILDDIR)/%.o: java/%.c
	@mkdir -p $(dir $@)
	$(CC) -c -o $@ $< $(CFLAGS) 2>>../BUILD/trlib/compile.err





$(EXENAME): $(OBJS)
	$(CC) -o $@ $< $(LDFLAGS) 

clean:
	rm -rf $(EXENAME) ../BUILD/trlib/
