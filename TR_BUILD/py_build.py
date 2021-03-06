#!/usr/bin/python 
#########################
## simple build instructions for TrLib (Mingw, gcc linux/mac and msvc windows).
## Run: py_build.py <trlib_dir> options......
## Assumes that we have subdisrs in trlib: TR_SRC, TR_INC, TR_SRC/API, TR_SRC/LORD
## simlk, sep. 2012
##############################
import sys,os,glob,shutil,subprocess
from core import *
import cc

OPTIONS={"-thread_safe": "Compile a thread safe library.",
"-debug":"Include debug symbols.",
"-debuggd":"Extra output from gd_trans.",
"-o": "<shared_object> :Specify output name.",
"-build": "Build shared libaray- default: modified source only.",
"-all":"Build all c-source.",
"-buildtest":"<path(s)> Build (single file) test programs in specified by <path(s)> (globbing allowed).",
"-pattern":" <pattern>  : Build files matching pattern.",
"-define":" comma separated list of defines...",
"-clean": "Delete objcet files.",
"-test":"Run tests.",
"-gprof":"Include symbols for gprof.",
"-compiler_version":"Echo compiler selection.",
"-java":"Build java (jni) bindings.",
"-java_external":"Build and link a separate library for jni-bindings.",
"-build_java":"Build java source (not implemented)."}
OPTIONS.update(COMPILER_SELECTION_OPTS)
#NEW: TRLIB ROOT is now fetched automatically
TRLIB=os.path.realpath(os.path.join(os.path.dirname(__file__),".."))
CWD=os.path.realpath(os.getcwd())
#THIS IS WHERE YOU WILL GET YOUR OBJECT FILES
BUILD_DIR=os.path.realpath("./BUILD_PY")
BUILD_DIR_CROSS=os.path.realpath("./BUILD_PY_W64")
#THIS IS WHERE YOU'LL GET YOUR OUTPUT LIBRARY BY DEFAULT
LIB_DIR=os.path.realpath("./LIB")
LIB_DIR_CROSS=os.path.realpath("./LIB64")
#SOURCE DIRS - WE ASSUME STANDARD STRUCTURE OF THE TRLIB ROOT DIR.
INC_DIR="TR_INC"
SRC_DIRS=["TR_SRC/GEOMETRIC_GEODESY","TR_SRC/PHYSICAL_GEODESY","TR_SRC/UTILITIES","TR_SRC/GENERIC_TRANSFORMATION","TR_SRC/SPECIFIC_TRANSFORMATION","TR_SRC/METADATA","TR_SRC/API","TR_SRC/LORD","TR_SRC/STRONG"]
DEF_FILE_API="TR_BUILD/trlib.def"
DEF_FILE_KMS_FNCS="KmsFncs.def"
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
	print("To run: %s options (at least one!)" %os.path.basename(sys.argv[0]))
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
	if len(lines)>1 or rc!=0:
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
	return map(lambda x:os.path.join(trlib,x),SRC_DIRS), [os.path.join(trlib,INC_DIR)]


def main(args):
	global TESTS
	if len(args)<2:
		Usage()
	if not os.path.exists(TRLIB) or not TestOptions(args[1:]):
		print TRLIB
		Usage()
	log_fp=open(BUILD_LOG,"w")
	sys.stdout=RedirectStdout(log_fp,True)
	compiler=SelectCompiler(args)
	if "-x64" in args:
		build_dir=BUILD_DIR_CROSS
		lib_dir=LIB_DIR_CROSS
	else:
		build_dir=BUILD_DIR
		lib_dir=LIB_DIR
	is_msvc=isinstance(compiler,cc.msvc)
	if "-compiler_version" in args:
		print GetCompiler(compiler)
	if "-o" in args:
		outname=args[args.index("-o")+1]
	else: 
		if "win" in sys.platform:
			outname=os.path.join(lib_dir,"trlib")
		else:
			outname=os.path.join(lib_dir,"libtr")
		outname+=DLL
	outname=os.path.realpath(outname)
	defines=DEFINES
	if "-debug" in args:
		defines.append("_ROUT")
		debug=True
	else:
		debug=False
	# #  # # # # # # # # # # # # # # #
	src_dirs,inc_dirs=GetSource(TRLIB)
	if "-build" in args:
		if "-define" in args:
			vals=args[args.index("-define")+1].split(",")
			defines.extend(vals)
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
		src_pat=args[args.index("-buildtest")+1]
		src_files=glob.glob(src_pat)
		if len(src_files)==0:
			print("No test input files to compile!")
			log_fp.close()
			return 1
		test_build=os.path.join(build_dir,"TEST")
		for name in src_files:
			src_name=os.path.realpath(name)
			basename,ext=os.path.splitext(os.path.basename(src_name))
			if (ext==".c"):
				test_name=basename+EXE
				ok=Build(compiler,test_name,[src_name],inc_dirs,is_debug=True,is_library=False,link_libraries=[outname],build_dir=test_build,link_all=False)
				if ok:
					TESTS.append(os.path.join(test_build,test_name))
			else:
				print("File: %s, bad extension %s" %basename,ext)
		
	if "-test" in args:
		#Well, outname should be findable by some ENV var.....
		for test in TESTS:
			print("Running %s" %test)
			os.system(test)
	if "-clean" in args:
		Clean(build_dir)
	log_fp.close()	
	return 0
	

if __name__=="__main__":
	sys.exit(main(sys.argv))

	


		
