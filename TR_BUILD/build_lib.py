#!/usr/bin/python 
#########################
## simple library build instructions - used to build java bindings for trlib
## Run: build_lib.py <source> -I <include1> -I <include2> ... -link <x1> -link <x2> ... -D <define1> .... -o <lib_out> -build <build_dir>
## simlk, sep. 2012
##############################
import sys,os,glob
from cc import *
from core import *
def Usage():
	print("%s <source> -I <include1> -I <include2> ... -link <x1> -link <x2> ... -D <define1> .... -o <lib_out> -build <build_dir>" %os.path.basename(sys.argv[0]))
	print("+ compiler selection options as in py_build.py")
	sys.exit(1)
	
def main(args):
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
			compiler=gcc_mac()
		else:
			compiler=gcc_nix()
	if "-build" in args:
		build_dir=args[args.index("-build")+1]
	else:
		build_dir=os.getcwd()
	if "-cc" in args:
		override=args[args.index("-cc")+1]
		compiler.overrideCompiler(override)
	if "-o" in args:
		outname=args[args.index("-o")+1]
	else: #improve here.... define extension in cc or core
		Usage()
	debug="-debug" in args
	if len(args)<2:
		Usage()
	source=glob.glob(args[1])
	if len(source)==0:
		Usage()
	print("Source: %s" %source)
	include=[]
	while "-I" in args:
		i=args.index("-I")
		include.append(args.pop(i+1))
		args.pop(i)
	print("Include: %s" %repr(include))
	link=[]
	while "-link" in args:
		i=args.index("-link")
		link.append(args.pop(i+1))
		args.pop(i)
	print("Link: %s" %repr(link))
	defines=[]
	while "-D" in args:
		i=args.index("-D")
		defines.append(args.pop(i+1))
		args.pop(i)
	print("Defines: %s" %repr(defines))
	ok=Build(compiler,outname,source,include,defines,debug,True,link,build_dir=build_dir,link_all=False)
	if ok:
		print("Succes!")
		return 0
	else:
		print("Failed!")
		return 1

if __name__=="__main__":
	sys.exit(main(sys.argv))
	
		