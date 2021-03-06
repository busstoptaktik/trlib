import sys,os,subprocess,glob
import cc
IS_WINDOWS=sys.platform.startswith("win")
IS_MAC="darwin" in sys.platform
ALL_OBJ=[".o",".obj"] #object files to remove on clean
if IS_WINDOWS:
	DLL=".dll"
	EXE=".exe"
elif IS_MAC:
	DLL=".dylib"
	EXE=""
else:
	DLL=".so"
	EXE=""
def RunCMD(cmd):
	new_cmd=[]
	cmd_str=""
	for item in cmd:
		item=item.strip()
		if len(item)>0:
			new_cmd.append(item)
			cmd_str+=item+" "
	print("%s\n" %cmd_str)
	if IS_WINDOWS:
		s=subprocess.Popen(new_cmd,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,shell=False)
	else:
		s=subprocess.Popen(cmd_str,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,shell=True)
	out=""
	while s.poll() is None:
		line=s.stdout.readline()
		if len(line.strip())>0:
			print line
			out+=line
	rc=s.poll()
	out+=s.stdout.read()
	return rc, out

def GetCompiler(compiler):
	rc,ver=RunCMD([compiler.COMPILER,compiler.VERSION_SWITCH])
	if rc==0:
		return ver.splitlines()[0]
	else:
		return "error"

def Build(compiler,outname,source,include=[],define=[],is_debug=False,is_library=True,link_libraries=[],def_file="",build_dir=".",link_all=True):
	cwd=os.getcwd()
	if (not isinstance(compiler,cc.ccompiler)):
		raise ValueError("Compiler must be a subclass of cc.ccompiler")
	#normalise paths - if not given as absolute paths...
	includes=map(lambda x:compiler.INCLUDE_SWITCH+os.path.realpath(x),include)
	defines=map(lambda x:compiler.DEFINE_SWITCH+x,define)
	source=map(os.path.realpath,source)
	#do not normalise link_libraries as it might contains a lot of 'non-path stuff' - use absolute paths her if you must - link_libraries=map(os.path.realpath,link_libraries)
	if len(def_file)>0:
		def_file=os.path.realpath(def_file)
	outname=os.path.realpath(outname)
	#end normalise paths
	#change path to buld dir
	build_dir=os.path.realpath(build_dir)
	if not os.path.exists(build_dir):
		os.makedirs(build_dir)
	os.chdir(build_dir)
	#fetch compile and link options...
	compile_options,link_options=compiler.getOptions(is_library,is_debug)
	compile=[compiler.COMPILER]+defines+compile_options+includes+source
	#import library and def-file....
	if IS_WINDOWS and is_library:
		if len(def_file)>0:
			def_file=compiler.DEF_FILE_SWITCH+def_file
		if compiler.IS_MSVC:
			implib=compiler.IMPLIB_SWITCH+os.path.splitext(outname)[0]+compiler.IMPLIB_EXT
		else:
			implib=""
	else:
		implib=""
		def_file=""
	
	outname=compiler.linkOutput(outname)
	#link all obj-files - perhaps use option to only link those just made? - depends on how builddir is used...#
	if link_all:
		obj_files=["*"+compiler.OBJ_EXTENSION]
	else:
		obj_files=[os.path.splitext(os.path.basename(fname))[0]+compiler.OBJ_EXTENSION for fname in source]
	if compiler.IS_MSVC:
		link_libraries=map(lambda x:x.replace(".dll",".lib"),link_libraries)
		link=[compiler.LINKER]+link_options+outname+[implib,def_file]+link_libraries+obj_files
	else:
		link=[compiler.LINKER]+link_options+outname+[implib]+obj_files+link_libraries+[def_file]
	if len(source)>0:
		rc,text=RunCMD(compile)
	else: #No modified files, I s'pose :-)
		print "No (modified?) source files... linking..."
		rc=0
	if rc==0:
		rc,text=RunCMD(link)
	os.chdir(cwd)
	if rc!=0:
		return False
	return True

def Clean(dir):
	print("Cleaning...")
	files=[]
	for ext in ALL_OBJ:
		files.extend(glob.glob(os.path.join(dir,"*"+ext)))
	for fname in files:
		os.remove(fname)

COMPILER_SELECTION_OPTS={"-msvc":"Use MSVC compiler (windows only).",
"-sunc":"Use sun c compiler",
"-x64":"Compile 64 bit binaries",
"-cc":"Override default compiler - e.g. 'gcc' - for example to use a gcc-like cross compiler.",
"-cop":"<args> Comma separated list of extra options passed on to build (object file compilation only)."
}

def SelectCompiler(args):
	compiler=None
	is_64="-x64" in args
	if "-msvc" in args:
		if is_64:
			compiler=cc.msvc64()
		else:
			compiler=cc.msvc32()
	elif  "-sunc" in args:
		if is_64:
			compiler=cc.sunc64()
		else:
			compiler=cc.sunc32()
	else: #defaults
		if IS_WINDOWS:
			if is_64:
				compiler=cc.mingw64()
			else:
				compiler=cc.mingw32()
		elif IS_MAC:
			compiler=cc.gcc_mac()
		else:
			compiler=cc.gcc_nix()
	if "-cc" in args:
		override=args[args.index("-cc")+1]
		compiler.overrideCompiler(override)
	#add extra compiler options#
	if "-cop" in args:
		compiler.ALL_BUILD+=args[args.index("-cop")].split(",")
	return compiler
	