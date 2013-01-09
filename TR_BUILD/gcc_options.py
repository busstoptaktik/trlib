import os,sys,shutil
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
if (not IS_WINDOWS):
	ALL_BUILD+=" -fPIC"
DEFAULT_LINK_OPTIONS="-shared" 
if IS_WINDOWS:
	DEFAULT_LINK_OPTIONS+=" -Wl,--kill-at"
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
#STANDARD BUILD OPTIONS
RELEASE_OPTIONS=ALL_BUILD+" -O3"
DEBUG_OPTIONS=ALL_BUILD+" -g -O -D_ROUT"
DEBUG_OPTIONS_TEST="-g -Wall -pedantic -O -D_ROUT"
if "-gprof" in sys.argv:
	DEBUG_OPTIONS_TEST+=" -pg"
if IS_WINDOWS:
	LINK_LIBRARIES="-lkernel32 -luser32 -lgdi32 -lwinspool -lshell32 -lole32 -loleaut32 -luuid -lcomdlg32 -ladvapi32"
else:
	LINK_LIBRARIES="-lm"
DEBUG_LINK=DEFAULT_LINK_OPTIONS+""
RELEASE_LINK=DEFAULT_LINK_OPTIONS+""






def BuildJNI(compiler,java_out,outname,options,source,include):
	compile_link="%s %s%s %s " %(compiler,LINK_OUTPUT_SWITCH,java_out,DEFAULT_LINK_OPTIONS)
	for inc_dir in include:
		compile_link+="%s%s " %(INCLUDE_SWITCH,inc_dir)
	compile_link+=options+" "+source+" "+outname
	print compile_link
	rc=os.system(compile_link)


