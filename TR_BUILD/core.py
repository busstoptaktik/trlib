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
	if not os.path.exists(build_dir):
		os.makedirs(build_dir)
	os.chdir(build_dir)
	includes=map(lambda x:compiler.INCLUDE_SWITCH+x,include)
	defines=map(lambda x:compiler.DEFINE_SWITCH+x,define)
	#fetch compile and link options...
	compile_options,link_options=compiler.getOptions(is_library,is_debug)
	compile=[compiler.COMPILER]+defines+compile_options+includes+source
	#import library and def-file....
	if IS_WINDOWS and is_library:
		if len(def_file)>0:
			def_file=compiler.DEF_FILE_SWITCH+def_file
		implib=compiler.IMPLIB_SWITCH+os.path.splitext(outname)[0]+compiler.IMPLIB_EXT
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