#########################
## simple build instructions for TrLib (Mingw-windows)
## Run: build.py <trlib_dir> options......
## Assumes that we have subdisrs in trlib: TR_SRC, TR_INC, TR_SRC/API 
## simlk, march 2012
##############################
import sys,os,glob,shutil,subprocess
if "-msvc" in sys.argv:
	from msvc_options import *
	IS_MSVC=True
else:
	from gcc_options import *
	IS_MSVC=False
OPTIONS={"-thread_safe": "Compile a thread safe library",
"-msvc":"Use MSVC compiler",
"-debug":"Include gdb debug symbols"
,"-debuggd":"Extra output from gd_trans",
"-o": "<shared_object> :Specify output name",
"-x64":"Use cross compiler",
"-x86":"Default compiler",
"-build": "Build something - default API only",
"-all":"Build all c-source",
"-buildtest":"Build test programs",
"-pattern":" <pattern>  : Build files matching pattern.",
"-clean": "Delete objcet files.",
"-test":"Run tests",
"-kmsfncs":"(DEPRECATED) Expose KE's functions (windows)",
"-gprof":"Include symbols for gprof",
"-compiler_version":"Echo compiler selection.",
"-java":"Build java (jni) bindings",
"-java_external":"Build and link a separate library for jni-bindings",
"-build_java":"Build java source"}
CWD=os.path.realpath(os.getcwd())
#THIS IS WHERE YOU WILL GET YOUR OBJECT FILES
BUILD_DIR=os.path.realpath("./BUILD_PY")
BUILD_DIR_CROSS=os.path.realpath("./BUILD_PY_W64")
#IF YOU HAVE TEST PROGRAMS TO COMPILE - PUT THEM HERE

#THIS IS WHERE YOU'LL GET YOUR OUTPUT LIBRARY BY DEFAULT
LIB_DIR=os.path.realpath("./LIB")
LIB_DIR_CROSS=os.path.realpath("./LIB64")
#SOURCE DIRS - WE ASSUME STANDARD STRUCTURE OF THE TRLIB ROOT DIR.
INC_DIR="TR_INC"
SRC_DIRS=["TR_SRC","TR_SRC/API","TR_SRC/LORD"]
DEF_FILE_API="TR_BUILD/trlib.def"
DEF_FILE_KMS_FNCS="KmsFncs.def"
TEST_DIR="APPS" #relative to trlib path
#THIS IS WHERE YOURE JAVA STUFF IS (SDK INCLUDE FILES ETC)  IS, IF AVAILABLE
JAVA_INC=["C:\\Programmer\\Java\\jdk1.7.0_07\\include","C:\\Programmer\\Java\\jdk1.7.0_07\\include\\win32"]
JAVA_COMPILER="javac"
JAVA_JNI_SRC="java/Interface.c"
JAVA_SRC="java/*.java"
#DEFINES#
DEFINES=""
#LOGFILE FOR BUILD
BUILD_LOG="py_build.log"
#ONLY FOR COMPILING THREAD TESTS - PUT YOUR THREAD LIBRARIES AND HEADERS IN THE LIB-DIR#
#AND NAME YOUR c-FILE WITH SOMETHING CONTAINING 'thread' - OTHERWISE NOTHING HAPPENS!#
if "win" in sys.platform:
	THREAD_LIB=os.path.join(LIB_DIR,"pthreadGC2.dll")
	THREAD_LIB_W64=os.path.join(LIB_DIR,"pthreadGC2-w64.dll")
else:
	THREAD_LIB="-lpthreads"
#PREDEFINED  TESTS TO RUN#
TESTS=[]


def Usage():
	print("To run: %s <path_to_source_dir> options (at least one!)" %os.path.basename(sys.argv[0]))
	print("Available options:")
	for option in OPTIONS.keys():
		print("%s : %s" %(option,OPTIONS[option]))
	sys.exit()

def TestOptions(opts):
	for option in opts:
		if option[0]=="-" and (option not in OPTIONS.keys()):
			print("Unrecognized option %s" %option)
			return False
	return True

def GetRev(trlib):
	rc,rev=RunCMD("hg identify -i %s" % trlib)
	return rev.strip()

def RunCMD(cmd):
	s=subprocess.Popen(cmd,stdout=subprocess.PIPE)
	out=""
	while s.poll() is None:
		line=s.stdout.readline()
		print line
		out+=line
	rc=s.poll()
	s.stdout.close()
	return rc, out

class RedirectStdout(object):
	def __init__(self,file_pointer,print_out=False):
		self.file_pointer=file_pointer
		self.print_out=print_out
	def write(self,text):
		self.file_pointer.write(text)
		if self.print_out:
			sys.__stdout__.write(text)
		

def GetCompiler(compiler=None,cross=False):
	if compiler is None:
		if cross:
			compiler=CROSS_COMPILER
		else:
			compiler=COMPILER
	rc,ver=RunCMD("%s %s" %(compiler,VERSION_SWITCH))
	if rc==0:
		return ver.splitlines()[0]
	else:
		return "error"

def GetCrossCompiler():
	return GetCompiler(cross=True)

def BuildLibrary(compiler,linker,outname,source,include,defines,options,link_options,def_file="",build_dir="."):
	if not os.path.exists(build_dir):
		os.mkdir(build_dir)
	os.chdir(build_dir)
	include_str=""
	for inc_dir in include:
		include_str+="%s%s" %(INCLUDE_SWITCH,inc_dir)
	compile="%s %s %s %s %s" %(compiler,defines,options,include_str,source)
	implib=os.path.splitext(outname)[0]+IMPLIB_EXT
	def_file=DEF_FILE_SWITCH+def_file
	implib=IMPLIB_SWITCH+implib
	outname=LINK_OUTPUT_SWITCH+outname
	#TODO: check gcc linker option like -g -O etc...
	if (IS_MSVC):
		link="%s %s %s %s %s %s %s" %(linker,link_options,outname,implib,def_file,LINK_LIBRARIES,OBJ_FILES)
	else:
		link="%s %s %s %s %s %s %s" %(linker,link_options,outname,LINK_LIBRARIES,implib,def_file,OBJ_FILES)
	#print compile
	#print link
	#return False
	rc,text=RunCMD(compile)
	if rc==0:
		print link
		rc,text=RunCMD(link)
	else:
		return False
	if rc!=0:
		return False
	return True

def BuildTest(compiler,outname,cfiles,include,link="",build_dir="."):
	tests=[]
	if not os.path.exists(build_dir):
		os.mkdir(build_dir)
	os.chdir(build_dir)
	include_str=""
	for inc_dir in include:
		include_str+="%s%s " %(INCLUDE_SWITCH,inc_dir)
	print("Compiling test program(s)...")
	for fname in cfiles:
		pname=LINK_OUTPUT_SWITCH+os.path.splitext(os.path.basename(fname))[0]+EXE_EXT
		if "thread" in fname.lower() and os.path.exists(link_libs):
			link_libs=link
		else:
			link_libs=""
		compile="%s %s %s %s %s %s %s" %(compiler,pname,DEBUG_OPTIONS,include_str,link_libs,fname,outname) 
		print compile
		rc,text=RunCMD(compile)
		if rc==0:
			tests.append(pname)
	
	
	return tests

	
	

	
def Clean(dir):
	print("Cleaning...")
	files=glob.glob(os.path.join(dir,OBJ_FILES))
	if OBJ_FILES!="*.obj":
		files.extend(glob.glob(os.path.join(dir,"*.obj")))
	elif OBJ_FILES!="*.o":
		files.extend(glob.glob(os.path.join(dir,"*.o")))
	for fname in files:
		os.remove(fname)
	

def GetSource(trlib):
	return map(lambda x:os.path.join(trlib,x),SRC_DIRS), [os.path.join(trlib,INC_DIR)], os.path.join(trlib,TEST_DIR)


def main(args):
	global TESTS
	if len(args)<3:
		Usage()
	TRLIB=os.path.realpath(args[1])
	if not os.path.exists(TRLIB) or not TestOptions(args[2:]):
		Usage()
	log_fp=open(BUILD_LOG,"w")
	sys.stdout=RedirectStdout(log_fp,True)
	if "-x64" in args:
		compiler=CROSS_COMPILER
		build_dir=BUILD_DIR_CROSS
		lib_dir=LIB_DIR_CROSS
		linker=CROSS_LINKER
		
	else:
		compiler=COMPILER
		build_dir=BUILD_DIR
		lib_dir=LIB_DIR
		linker=LINKER
	if "-compiler_version" in args:
		print GetCompiler(compiler)
	#ALWAYS PUT LIBRARY IN LIBDIR#
	if "-o" in args:
		outname=sys.argv[sys.argv.index("-o")+1]
	else:
		if "win" in sys.platform:
			outname=os.path.join(lib_dir,"TrLib.dll")
		else:
			outname=os.path.join(lib_dir,"TrLib.so")
	outname=os.path.realpath(outname)
	if "-debug" in args:
		options=DEBUG_OPTIONS
		link_options=DEBUG_LINK
	else:
		options=RELEASE_OPTIONS
		link_options=RELEASE_LINK
	if "-gprof" in args and not IS_MSVC:
		options+=" -pg"
	# #  # # # # # # # # # # # # # # #
	src_dirs,inc_dirs,test_dir=GetSource(TRLIB)
	if "-build" in args:
		outdir=os.path.dirname(outname)
		if not os.path.exists(outdir):
			os.mkdir(outdir)
		defines=DEFINES
		defines+=""" %sTRLIB_REVISION=\\"%s\\" """ %(DEFINE_SWITCH,GetRev(TRLIB))
		if "-debuggd" in args:
			defines+=" %sDEBUGGDTRANS" %DEFINE_SWITCH
		if "-pattern" in args:
			pat=args[args.index("-pattern")+1]
			source=""
			for dir in src_dirs:
				s_files=glob.glob(os.path.join(dir,pat))
				for name in s_files:
					source+=name+" "
		else:
			source=""
			if "-all" in args:
				for dir in src_dirs:
					source+=" "+os.path.join(dir,"*.c")
			else:
				source=os.path.join(src_dirs[1],"*.c")
			if "-java" in args:
				source+=" "+os.path.join(TRLIB,JAVA_JNI_SRC)
				inc_dirs+=JAVA_INC
		if "-thread_safe" in args:
			defines+=" %sTHREAD_SAFE" %DEFINE_SWITCH
		
		if IS_MSVC:
			if "-kmsfncs" in args:
				print("DEPRECATED OPTION....")
				def_file=os.path.join(TRLIB,DEF_FILE_KMS_FNCS)
			else:
				def_file=os.path.join(TRLIB,DEF_FILE_API)
		else:
			def_file=""
		ok=BuildLibrary(compiler,linker,outname,source,inc_dirs,defines,options,link_options,def_file,build_dir)
		if not ok:
			print("Error, exiting")
			log_fp.close()
			sys.exit()
	
	if "-java_external" in args:
		base,ext=os.path.splitext(outname)
		java_out=base+"_java"+ext
		BuildJNI(compiler,java_out,outname,options,os.path.join(TRLIB,JAVA_JNI_SRC),JAVA_INC+inc_dirs) 
	if ("-buildtest" in args):
		if not os.path.exists(test_dir):
			print("%s does not exist!" %test_dir)
			log_fp.close()
			sys.exit()
		test_build=os.path.join(build_dir,"TEST")
		cfiles=glob.glob(os.path.join(test_dir,"*.c"))
		if "-x64" in args:
			thread_lib=THREAD_LIB_W64
		else:
			thread_lib=THREAD_LIB
		
		#shutil.copy(outname,os.path.join(TEST_DIR,os.path.basename(outname)))
		if IS_MSVC:
			libname=os.path.splitext(outname)[0]+IMPLIB_EXT
		else:
			libname=outname
		TESTS.extend(BuildTest(compiler,libname,cfiles,inc_dirs,thread_lib,test_build))
		os.chdir(CWD)
	if "-test" in args:
		for test in TESTS:
			print("Running %s" %test)
			os.system(test)
	if "-clean" in args:
		Clean(build_dir)
	log_fp.close()	
	sys.exit()
	

if __name__=="__main__":
	main(sys.argv)

	


		
