#MUST DEFINE:
#COMPILER
#LINKER
#INCLUDE_SWITCH
#DEFINE_SWITCH
#IMPLIB_SWITCH
#DEF_FILE_SWITCH
#LINK_OUTPUT_SWITCH
#IMPLIB_EXT
#OBJ_FILES
#COMPILE_LIBRARY_RELEASE
#COMPILE_LIBRARY_DEBUG
#COMPILE_EXE_RELEASE
#COMPILE_EXE_DEBUG
#LINK_LIBRARY_RELEASE
#LINK_LIBRARY_DEBUG
#LINK_EXE_RELEASE
#LINK_EXE_DEBUG
#LINK_LIBRARIES
#VERSION_SWITCH
import os
#DEFAULT COMPILERS AND SWITCHES#
IS_WINDOWS=sys.platform.startswith("win")
if "darwin" in sys.platform:
	COMPILER="/opt/local/bin/gcc-mp-4.6" #perhaps add a function to find a proper compiler....
else:
	COMPILER="gcc"
CROSS_COMPILER="x86_64-w64-mingw32-gcc.exe" #REMEMBER to include this in your path!
DEFINE_SWITCH="-D"
INCLUDE_SWITCH="-I"
ALL_BUILD="-c -Wall -pedantic"
COMPILE_LIBRARY_RELEASE=ALL_BUILD+" -O3"
COMPILE_LIBRARY_DEBUG=ALL_BUILD+" -g -O"
COMPILE_EXE_RELEASE=ALL_BUILD+" -O3"
COMPILE_EXE_DEBUG=ALL_BUILD+" -g -O"
if (not IS_WINDOWS):
	COMPILE_LIBRARY_RELEASE+=" -fPIC"
	COMPILE_LIBRARY_DEBUG+=" -fPIC"
LINK_LIBRARY_RELEASE="-shared"
LINK_LIBRARY_DEBUG="-shared"
if IS_WINDOWS:
	LINK_LIBRARY_DEBUG+=" -Wl,--kill-at"
	LINK_LIBRARY_RELEASE+=" -Wl,--kill-at"
LINK_OUTPUT_SWITCH="-o"
EXE_OUTPUT_SWITCH="-o"
VERSION_SWITCH="--version"
DEF_FILE_SWITCH=""
IMPLIB_SWITCH="-Wl,--out-implib,"
IMPLIB_EXT=".a"
LINKER=COMPILER
CROSS_LINKER=CROSS_COMPILER
#OBJ FILES#
OBJ_FILES="*.o"
if IS_WINDOWS:
	LINK_LIBRARIES="-lkernel32 -luser32 -lgdi32 -lwinspool -lshell32 -lole32 -loleaut32 -luuid -lcomdlg32 -ladvapi32"
else:
	LINK_LIBRARIES="-lm"







def BuildJNI(compiler,java_out,outname,options,source,include):
	compile_link="%s %s%s %s " %(compiler,LINK_OUTPUT_SWITCH,java_out,DEFAULT_LINK_OPTIONS)
	for inc_dir in include:
		compile_link+="%s%s " %(INCLUDE_SWITCH,inc_dir)
	compile_link+=options+" "+source+" "+outname
	print compile_link
	rc=os.system(compile_link)


