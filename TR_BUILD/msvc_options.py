import os,sys,shutil
#DEFAULT COMPILERS AND SWITCHES#
COMPILER="cl"
CROSS_COMPILER="cl"
LINK_OUTPUT_SWITCH="/out:"
DEFINE_SWITCH="/D"
INCLUDE_SWITCH="/I"
VERSION_SWITCH=""
DEF_FILE_SWITCH="/def:"
IMPLIB_SWITCH="/implib:"
IMPLIB_EXT=".lib"
LINKER="link"
CROSS_LINKER="link"
#OBJ FILES#
OBJ_FILES="*.obj"
#STANDARD BUILD OPTIONS
ALL_BUILD="/c /D_WINDOWS /W3 /Zm1000 /TC /fp:precise /D_CRT_SECURE_NO_WARNINGS"
DEFAULT_LINK_OPTIONS="/DLL"
RELEASE_OPTIONS=ALL_BUILD+" /MD /O2 /Ob2 /D NDEBUG"
DEBUG_OPTIONS=ALL_BUILD+" /D_ROUT /D_DEBUG /MDd /Zi /Ob0 /Od /RTC1"
DEBUG_OPTIONS_TEST="/D_ROUT /D_DEBUG /W3"
LINK_LIBRARIES='kernel32.lib  user32.lib  gdi32.lib  winspool.lib  shell32.lib  ole32.lib  oleaut32.lib  uuid.lib  comdlg32.lib  advapi32.lib'
#LINK_LIBRARIES=""
RELEASE_LINK=DEFAULT_LINK_OPTIONS+" /INCREMENTAL:NO"
DEBUG_LINK=DEFAULT_LINK_OPTIONS+" /debug /INCREMENTAL"





#~ def BuildLibrary(compiler,linker,outname,source,include,defines,options,link_options,def_file="",build_dir="."):
	#~ if not os.path.exists(build_dir):
		#~ os.mkdir(build_dir)
	#~ os.chdir(build_dir)
	#~ compile="%s /c %s %s /I%s %s" %(compiler,defines,options,include,source)
	#~ implib=os.path.splitext(outname)[0]+".lib"
	#~ link="%s /DLL %s /out:%s /implib:%s /def:%s %s *.obj" %(linker,link_options,outname,implib,def_file,LINK_LIBRARIES)
	#~ print compile
	#~ rc=os.system(compile)
	#~ if rc==0:
		#~ print link
		#~ rc=os.system(link)
	#~ else:
		#~ return False
	#~ if rc!=0:
		#~ return False
	#~ return True

def BuildJNI(compiler,java_out,outname,options,source,include):
	compile_link="%s /LD  /Fe%s " %(compiler,java_out)
	for inc_dir in include:
		compile_link+="/I %s " %inc_dir
	implib=os.path.splitext(outname)[0]+".lib"
	compile_link+=options+" "+source+" /link "+implib+" %s"%(RELEASE_LINK)
	print compile_link
	rc=os.system(compile_link)
	return rc

#~ def BuildTest(compiler,outname,cfiles,include,link="",build_dir="."):
	#~ tests=[]
	#~ if not os.path.exists(build_dir):
		#~ os.mkdir(build_dir)
	#~ os.chdir(build_dir)
	#~ print("Compiling test program(s)...")
	#~ implib=os.path.splitext(outname)[0]+".lib"
	#~ for fname in cfiles:
		#~ pname=os.path.splitext(fname)[0]+".exe"
		#~ compile="%s /I%s %s %s" %(compiler,include,fname,implib) 
		#~ print compile
		#~ rc=os.system(compile)
		#~ if rc==0:
			#~ tests.append(pname)
	#~ return tests
