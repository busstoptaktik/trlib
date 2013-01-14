#!/usr/bin/python 
#########################
## simple build instructions for TrLib (Mingw, gcc linux/mac and msvc windows).
## Run: py_build.py <trlib_dir> options......
## Assumes that we have subdisrs in trlib: TR_SRC, TR_INC, TR_SRC/API, TR_SRC/LORD
## simlk, sep. 2012
##############################
import sys,os,glob,shutil,subprocess
from cc import *
from core import *
OPTIONS={"-thread_safe": "Compile a thread safe library.",
"-msvc":"Use MSVC compiler (windows only).",
"-debug":"Include debug symbols.",
"-debuggd":"Extra output from gd_trans.",
"-o": "<shared_object> :Specify output name.",
"-x64":"Use cross compiler (windows only).",
"-x86":"Default compiler (windows only).",
"-build": "Build shared libaray- default: modified source only.",
"-all":"Build all c-source.",
"-buildtest":"Build test programs.",
"-pattern":" <pattern>  : Build files matching pattern.",
"-clean": "Delete objcet files.",
"-test":"Run tests.",
"-kmsfncs":"(DEPRECATED) Expose KE's functions (windows).",
"-gprof":"Include symbols for gprof.",
"-compiler_version":"Echo compiler selection.",
"-cc":"Override default compiler - e.g. 'gcc' - for example to use a gcc-like cross compiler.",
"-java":"Build java (jni) bindings.",
"-java_external":"Build and link a separate library for jni-bindings.",
"-build_java":"Build java source (not implemented)."}
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
SRC_DIRS=["TR_SRC","TR_SRC/API","TR_SRC/LORD","TR_SRC/PARSING","TR_SRC/UTILS"]
DEF_FILE_API="TR_BUILD/trlib.def"
DEF_FILE_KMS_FNCS="KmsFncs.def"
TEST_DIR="APPS" #relative to trlib path
#THIS IS WHERE YOURE JAVA STUFF IS (SDK INCLUDE FILES ETC)  IS, IF AVAILABLE
JAVA_INC=["C:\\Programmer\\Java\\jdk1.7.0_07\\include","C:\\Programmer\\Java\\jdk1.7.0_07\\include\\win32"]
JAVA_JNI_SRC=["java/Interface.c"]
#DEFINES#
DEFINES=[]
#LOGFILE FOR BUILD
BUILD_LOG="py_build.log"
#ONLY FOR COMPILING THREAD TESTS - PUT YOUR THREAD LIBRARIES AND HEADERS IN THE LIB-DIR#
#AND NAME YOUR c-FILE WITH SOMETHING CONTAINING 'thread' - OTHERWISE NOTHING HAPPENS!#
if sys.platform.startswith("win"):
	IS_WINDOWS=True
	THREAD_LIB=os.path.join(LIB_DIR,"pthreadGC2.dll")
	THREAD_LIB_W64=os.path.join(LIB_DIR,"pthreadGC2-w64.dll")
	EXE_EXT=".exe"
else:
	IS_WINDOWS=False
	THREAD_LIB="-lpthreads"
	EXE_EXT=".out"
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
	try:
		rc,rev=RunCMD(["hg","identify","-i",trlib])
	except:
		print "Identify failed - trlib is: %s" %trlib
		rev="undefined"
	lines=rev.splitlines()
	if len(lines)>1:
		print "Identify failed - trlib is: %s" %trlib
		return "undefined"
	return rev.strip()
	
def GetModified(src_dirs,build_dir,obj_extension):
	source=[]
	for dir in src_dirs:
		files=glob.glob(os.path.join(dir,"*.c"))
		for fname in files:
			t_src=os.path.getmtime(fname)
			obj_name=os.path.join(build_dir,os.path.splitext(os.path.basename(fname))[0]+obj_extension)
			if os.path.exists(obj_name):
				t_obj=os.path.getmtime(obj_name)
			else:
				t_obj=-1
			if t_src>t_obj:
				source.append(fname)
	return source
	

class RedirectStdout(object):
	def __init__(self,file_pointer,print_out=False):
		self.file_pointer=file_pointer
		self.print_out=print_out
	def write(self,text):
		self.file_pointer.write(text)
		if self.print_out:
			sys.__stdout__.write(text)
		



def BuildTest(compiler,outname,cfiles,include,link="",build_dir="."):
	#TODO
	return []

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
	is_msvc="-msvc" in args
	if is_msvc:
		if "-x64" in args:
			compiler=msvc64()
		else:
			compiler=msvc32()
	else:
		if "-x64" in args:
			compiler=mingw64()
		elif IS_WINDOWS:
			compiler=mingw32()
		elif IS_MAC:
			compiler=macports_gcc()
		else:
			compiler=gcc_nix()
	if "-x64" in args:
		build_dir=BUILD_DIR_CROSS
		lib_dir=LIB_DIR_CROSS
	else:
		build_dir=BUILD_DIR
		lib_dir=LIB_DIR
	if "-cc" in args:
		override=args[args.index("-cc")+1]
		compiler.overrideCompiler(override)
	if "-compiler_version" in args:
		print GetCompiler(compiler)
	#ALWAYS PUT LIBRARY IN LIBDIR#
	if "-o" in args:
		outname=args[args.index("-o")+1]
	else: #improve here.... define extension in cc or core
		if "win" in sys.platform:
			outname=os.path.join(lib_dir,"TrLib.dll")
		else:
			outname=os.path.join(lib_dir,"TrLib.so")
	outname=os.path.realpath(outname)
	defines=DEFINES
	if "-debug" in args:
		defines.append("_ROUT")
		debug=True
	else:
		debug=False
	# #  # # # # # # # # # # # # # # #
	src_dirs,inc_dirs,test_dir=GetSource(TRLIB)
	if "-build" in args:
		outdir=os.path.dirname(outname)
		if not os.path.exists(outdir):
			os.mkdir(outdir)
		print("Building source in %s at revision:" %TRLIB)
		rev=GetRev(TRLIB)
		defines.append("TRLIB_REVISION=%s"%(rev))
		if "-debuggd" in args:
			defines.append("DEBUGGDTRANS")
		if "-pattern" in args:
			pat=args[args.index("-pattern")+1]
			source=[]
			for dir in src_dirs:
				source.extend(glob.glob(os.path.join(dir,pat)))
		else:
			if "-all" in args:
				source=map(lambda x:os.path.join(x,"*.c"),src_dirs)
			else:
				source=GetModified(src_dirs,build_dir,compiler.OBJ_EXTENSION)
			if "-java" in args:
				source+=map(lambda x:os.path.join(TRLIB,x),JAVA_JNI_SRC)
				inc_dirs+=JAVA_INC
			
		if "-thread_safe" in args:
			defines.append("THREAD_SAFE")
		if is_msvc:
			if "-kmsfncs" in args:
				print("DEPRECATED OPTION....")
				def_file=os.path.join(TRLIB,DEF_FILE_KMS_FNCS)
			else:
				def_file=os.path.join(TRLIB,DEF_FILE_API)
		else:
			def_file=""
		ok=Build(compiler,outname,source,inc_dirs,defines,debug,True,compiler.LINK_LIBRARIES,def_file,build_dir)
		if not ok:
			print("Error, exiting")
			log_fp.close()
			return 1
	#TODO: fix below here...
	if ("-buildtest" in args):
		if not os.path.exists(test_dir):
			print("%s does not exist!" %test_dir)
			log_fp.close()
			return 1
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
		TESTS.extend(BuildTest(compiler,libname,cfiles,inc_dirs,[thread_lib],test_build))
		os.chdir(CWD)
	if "-test" in args:
		for test in TESTS:
			print("Running %s" %test)
			os.system(test)
	if "-clean" in args:
		Clean(build_dir)
	log_fp.close()	
	return 0
	

if __name__=="__main__":
	sys.exit(main(sys.argv))

	


		
