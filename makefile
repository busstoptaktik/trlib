#
# MAKEFILE:  For KMS transformations bibliotek, version 1.1
#
#
# Eksempler på kald:
#
# 1. Nyt bibliotek (libtr.dll libtr.lib) med funktionerne i makefilen:
#
#    make all
#
# 2. word count på alle funktionerne
#
#    make wordcount
#
	
MAKEFILE           = Makefile


# GST transformations funktioner (det bibliotek vi ønsker at danne)
TR_VERSION          =  1
TR_MINOR_VERSION    =  1
TRLIBRARY           = trlib.${TR_VERSION}.${TR_MINOR_VERSION}
TRLIB_NAME          = libtr
LIBTR               = ${TRLIB_NAME}.dll
LINTLIBTR           = -l${TRLIBRARY}

BUILDDIR            = c:/Work/Projects/BUILD/${TRLIB_NAME}

CC                 = gcc
CCOPTION           = -w -pedantic -fPIC ${CCWARNING}
CFLAGS             = -O -std=c99 -mdll -mwin32
CCWARNING          = -Wall

# Simon skriver om D_ROUT. Tænder debug mode giver nogle ekstra meddelelser og laver en fil TR_Errlog.log hvor tingene skrives ud i. 
# Så er nok heller ikke nødvendig i release udgaver men måske praktisk til aftestning. Sættes i CFLAGS

#-pthread
LD                 = ${CC}
LDFLAGS            = -L. -shared -DTHREAD_SAFE


# ar and flags
# ar og flag
ARFLAGS             = rvs 
ARLISTFLAG          = tv

# Compiler flags
LD_LIBRARY_PATH    =  c:/Work/Projects/BUILD/${TRLIBRARY}
LD_LIBRARY_PATH_64 =  


# path til projekt
PROJ                = c:/SRC/trlib
# projekt kilder
SRC                 = TR_SRC
SRC_APPS	    = apps
INC      	    = TR_INC
# path til projekt kilde koder
PROJ_SRC_PATH       = ${PROJ}/${SRC}
# path til projekt kilde headere
PROJ_INC_PATH       = ${PROJ}/${INC}/
INC_LIBTR_DEP	    = ${PROJ_INC_PATH}/DEPRECATED/

ALL_HEADERS         = -I${PROJ_INC_PATH}


# drop: Buffer overflow possible with sprintf.  Recommend using snprintf instead: sprintf
#       Use of function that may lead to buffer overflow. (Use -bufferoverflowhigh to inhibit warning)
# drop: Format argument 1 to fprintf (%ld) expects long int gets size_t: strlen(select_1)
#       To allow arbitrary integral types to match long unsigned, use +longintegral.
# drop: A function or variable is redefined. One of the declarations should use extern. (Use -redef to inhibit warning)
# drop: A variable declared outside a macro body starts with the macrovarprefix.
#       (Use either -macrovarprefixexclude or -namechecks to
#

GEOMETRIC_GEODESYSRC       = ${PROJ_SRC_PATH}/GEOMETRIC_GEODESY/*c
PHYSICAL_GEODESYSRC        = ${PROJ_SRC_PATH}/PHYSICAL_GEODESY/*c
UTILITIESSSRC              = ${PROJ_SRC_PATH}/UTILITIES/*.c
GENERIC_TRANSFORMATIONSRC  = ${PROJ_SRC_PATH}/GENERIC_TRANSFORMATION/*.c
SPECIFIC_TRANSFORMATIONSRC = ${PROJ_SRC_PATH}/SPECIFIC_TRANSFORMATION/*.c
METADATASRC                = ${PROJ_SRC_PATH}/METADATA/*c
LORDSRC                    = ${PROJ_SRC_PATH}/LORD/*c
APISRC                     = ${PROJ_SRC_PATH}/API/*.c
JNISRC                     = ${PROJ_SRC_PATH}/java/*.c
STRONGSRC                  = ${PROJ_SRC_PATH}/STRONG/*.c

ALLSRC                     = $(GEOMETRIC_GEODESYSRC) $(PHYSICAL_GEODESYSRC) $(UTILITIESSSRC) $(GENERIC_TRANSFORMATIONSRC) $(SPECIFIC_TRANSFORMATIONSRC) $(METADATASRC) $(LORDSRC) $(APISRC) $(STRONGSRC)

all: lib cleanup

lib: pre objects

pre:
	@echo "start of pre"
	@-rm compile.err
	@-rm *.o
	@-rm ${LIBTR}
	@-rm $(TRLIB_NAME).lib
	@echo "end of pre"

objects :
	@echo "Compiling source files"        
	@echo "Compiling source files" >> compile.err       
	@echo "----------------------" >> compile.err       
	@-${CC} -c ${ALL_HEADERS} ${CFLAGS} ${CCOPTION} ${ALLSRC} 2>>compile.err
	@echo "exports source files"        
	@${AR} ${ARFLAGS} ${LIBTR} *.o
	@-dlltool -d Kmsfncs.def -l ${TRLIB_NAME}.lib

cleanup:
	@echo "Cleaning object files"
	@-rm *.o

# Funktioner i biblioteket ved ar
#lslib:
#	@echo "Functions in library"
#	@if [ -s ${LIBTR} ] ; \
#	then ${AR} ${ARLISTFLAG} ${LIBTR} ; \
#	else \
#	echo "Function library ${LIBTR} does not exist" ; \
#	fi


#
## word count on functions
wordcount:
	@echo "wordcount on C functions "
	@echo "  linier    ord    tegn fil "
	@wc ${ALLSRC}


