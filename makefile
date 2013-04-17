BUILDDIR = ../BUILD/trlib
CC = gcc
CFLAGS = -W -Wall -pedantic -fPIC -O -pg -g -I./TR_INC -I./TR_INC/DEPRECATED -DTHREAD_SAFE -D_ROUT
#-pthread
LDFLAGS = -I./TR_INC -shared  -DTHREAD_SAFE -pg -g
#-pthread
 
EXENAME = main

SRCS       =  $(wildcard TR_SRC/*.c)
GEOMETRIC_GEODESYSRC    = $(wildcard TR_SRC/GEOMETRIC_GEODESY/*c)
PHYSICAL_GEODESYSRC    = $(wildcard TR_SRC/PHYSICAL_GEODESY/*c)
UTILITIESSSRC   =  $(wildcard TR_SRC/UTILITIES/*.c)
GENERIC_TRANSFORMATIONSRC   =  $(wildcard TR_SRC/GENERIC_TRANSFORMATION/*.c)
SPECIFIC_TRANSFORMATIONSRC   =  $(wildcard TR_SRC/SPECIFIC_TRANSFORMATION/*.c)
LORDSRC    = $(wildcard TR_SRC/LORD/*c)
DEPSRC     =  $(wildcard TR_SRC/DEPRECATED/*.c)
APISRC     =  $(wildcard TR_SRC/API/*.c)
JNISRC     =  $(wildcard java/*.c)
PARSINGSRC =  $(wildcard TR_SRC/PARSING/*.c)
UTILSSRC   =  $(wildcard TR_SRC/UTILS/*.c)
STRONGSRC  =  $(wildcard TR_SRC/STRONG/*.c)

#ALLSRC =  $(SRCS) $(LORDSRC) $(APISRC) $(JNISRC) $(DEPSRC)
ALLSRC =  $(SRCS) $(GEOMETRIC_GEODESYSRC) $(PHYSICAL_GEODESYSRC) $(UTILITIESSRC) $(GENERIC_TRANSFORMATION) $(SPECIFIC_TRANSFORMATION) $(LORDSRC) $(DEPSRC) $(APISRC) $(UTILSSRC) $(PARSINGSRC) $(STRONGSRC)

OBJS       =  $(patsubst TR_SRC/%.c,     $(BUILDDIR)/%.o,  $(SRCS))
GEOMETRIC_GEODESYOBJ =  $(patsubst TR_SRC/GEOMETRIC_GEODESY/%.c,     $(BUILDDIR)/%.o,  $(GEOMETRIC_GEODESYSRC))
PHYSICAL_GEODESYOBJ =  $(patsubst TR_SRC/PHYSICAL_GEODESY/%.c,     $(BUILDDIR)/%.o,  $(PHYSICAL_GEODESYSRC))
UTILITIESOBJ   =  $(patsubst TR_SRC/UTILS/%.c,   $(BUILDDIR)/%.o,  $(UTILITIESSRC))
GENERIC_TRANSFORMATIONOBJ   =  $(patsubst TR_SRC/GENERIC_TRANSFORMATION/%.c,   $(BUILDDIR)/%.o,  $(GENERIC_TRANSFORMATIONSRC))
SPECIFIC_TRANSFORMATIONOBJ   =  $(patsubst TR_SRC/SPECIFIC_TRANSFORMATION/%.c,   $(BUILDDIR)/%.o,  $(SPECIFIC_TRANSFORMATIONSRC))
LORDOBJ    =  $(patsubst TR_SRC/LORD/%.c,     $(BUILDDIR)/%.o,  $(LORDSRC))
APIOBJ     =  $(patsubst TR_SRC/API/%.c, $(BUILDDIR)/%.o,  $(APISRC))
DEPOBJ     =  $(patsubst TR_SRC/DEPRECATED/%.c, $(BUILDDIR)/%.o,  $(DEPSRC))
JNIOBJ     =  $(patsubst java/%.c,       $(BUILDDIR)/%.o,  $(JNISRC))
PARSINGOBJ =  $(patsubst TR_SRC/PARSING/%.c, $(BUILDDIR)/%.o,  $(PARSINGSRC))
UTILSOBJ   =  $(patsubst TR_SRC/UTILS/%.c,   $(BUILDDIR)/%.o,  $(UTILSSRC))
STRONGOBJ   =  $(patsubst TR_SRC/STRONG/%.c,   $(BUILDDIR)/%.o,  $(STRONGSRC))
#ALLOBJ =  $(OBJS) $(LORDOBJ) $(APIOBJ) $(JNIOBJ) $(DEPOBJ)  $(UTILSOBJ) $(PARSINGOBJ)
ALLOBJ =  $(OBJS) $(GEOMETRIC_GEODESYOBJ) $(PHYSICAL_GEODESYOBJ) $(UTILITIESOBJ) $(GENERIC_TRANSFORMATION) $(SPECIFIC_TRANSFORMATION) $(LORDOBJ) $(DEPOBJ) $(APIOBJ) $(UTILSOBJ) $(PARSINGOBJ) $(STRONGOBJ)
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
