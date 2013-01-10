BUILDDIR = ../BUILD/trlib
CC = gcc
CFLAGS = -W -Wall -pedantic -fPIC -O -pg -g -I./TR_INC -I./TR_INC/DEPRECATED -DTHREAD_SAFE -D_ROUT
#-pthread
LDFLAGS = -I./TR_INC -shared  -DTHREAD_SAFE -pg -g
#-pthread
 
EXENAME = main

SRCS       =  $(wildcard TR_SRC/*.c)
LORDSRC    = $(wildcard TR_SRC/LORD/*c)
DEPSRC     =  $(wildcard TR_SRC/DEPRECATED/*.c)
APISRC     =  $(wildcard TR_SRC/API/*.c)
JNISRC     =  $(wildcard java/*.c)
PARSINGSRC =  $(wildcard TR_SRC/PARSING/*.c)
UTILSSRC   =  $(wildcard TR_SRC/UTILS/*.c)

#ALLSRC =  $(SRCS) $(LORDSRC) $(APISRC) $(JNISRC) $(DEPSRC)
ALLSRC =  $(SRCS) $(LORDSRC) $(DEPSRC) $(APISRC) $(UTILSSRC) $(PARSINGSRC)

OBJS       =  $(patsubst TR_SRC/%.c,     $(BUILDDIR)/%.o,  $(SRCS))
LORDOBJ    =  $(patsubst TR_SRC/LORD/%.c,     $(BUILDDIR)/%.o,  $(LORDSRC))
APIOBJ     =  $(patsubst TR_SRC/API/%.c, $(BUILDDIR)/%.o,  $(APISRC))
DEPOBJ     =  $(patsubst TR_SRC/DEPRECATED/%.c, $(BUILDDIR)/%.o,  $(DEPSRC))
JNIOBJ     =  $(patsubst java/%.c,       $(BUILDDIR)/%.o,  $(JNISRC))
PARSINGOBJ =  $(patsubst TR_SRC/PARSING/%.c, $(BUILDDIR)/%.o,  $(PARSINGSRC))
UTILSOBJ   =  $(patsubst TR_SRC/UTILS/%.c,   $(BUILDDIR)/%.o,  $(UTILSSRC))
#ALLOBJ =  $(OBJS) $(LORDOBJ) $(APIOBJ) $(JNIOBJ) $(DEPOBJ)  $(UTILSOBJ) $(PARSINGOBJ)
ALLOBJ =  $(OBJS) $(LORDOBJ) $(DEPOBJ) $(APIOBJ) $(UTILSOBJ) $(PARSINGOBJ)

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
	$(CC) -c -o $@ $< $(CFLAGS) 2>>$(BUILDDIR)/compile.err
$(BUILDDIR)/%.o: TR_SRC/LORD/%.c
	@mkdir -p $(dir $@)
	$(CC) -c -o $@ $< $(CFLAGS) 2>>$(BUILDDIR)/compile.err
$(BUILDDIR)/%.o: TR_SRC/API/%.c
	@mkdir -p $(dir $@)
	$(CC) -c -o $@ $< $(CFLAGS) 2>>$(BUILDDIR)/compile.err
$(BUILDDIR)/%.o: TR_SRC/DEPRECATED/%.c
	@mkdir -p $(dir $@)
	$(CC) -c -o $@ $< $(CFLAGS) 2>>$(BUILDDIR)/compile.err
$(BUILDDIR)/%.o: java/%.c
	@mkdir -p $(dir $@)
	$(CC) -c -o $@ $< $(CFLAGS) 2>>$(BUILDDIR)/compile.err





$(EXENAME): $(OBJS)
	$(CC) -o $@ $< $(LDFLAGS) 

clean:
	rm -rf $(EXENAME) $(BUILDDIR)
