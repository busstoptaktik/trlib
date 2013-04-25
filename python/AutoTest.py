#!/usr/bin/python
###########################
## Complete python autotestsuite for TrLib 
## Wrapper of several individual tests
## Needs a job-definition file auto_tests.txt for file transformations
## simlk, oct. 2011
############################
import TrLib
import TestFiles
import RandomTests
import ThreadTest
import TestProj
import time,os,sys,platform
TEST_DATA=os.path.join(os.path.dirname(__file__),os.path.join("..","testdata"))
JOB_DEF=os.path.join(TEST_DATA,"job_new.txt")
LOG_FILE="auto_test"
PROG=os.path.basename(sys.argv[0])
LS="*"*80+"\n"
class RedirectStdout(object):
	def __init__(self,fp=None):
		self.fp=fp
	def write(self,text):
		sys.__stdout__.write(text)
		if self.fp is not None:
			try:
				self.fp.write(text)
			except:
				pass

def Usage():
	print("To run:\n%s <shared_library> <geoid_dir> -no_threads" %(PROG))
	print("<geoid_dir> is optional, will default to current working dir or dir set in: %s" %TrLib.TABDIR_ENV)
	print("-no_threads argument is optional. Use to disable thread safety test.")
	print("Will read the job definition file: %s" %JOB_DEF)
	sys.exit()

def main(args): #progname, test_lib
	i=args.index("-no_threads")
	if (i!=-1):
		args.pop(i)
		do_threads=False
	else:
		do_threads=True
	if len(args)<2:
		Usage()
	lib=args[1]
	lib_dir=os.path.dirname(lib)
	lib_obj=os.path.basename(lib)
	if len(args)>2:
		geoids=args[2]
	else:
		geoids=""
	now=time.asctime()
	nows=now.replace(" ","_").replace(":","_")
	log_name=LOG_FILE+"_"+nows+".log"
	log_fp=open(log_name,"w")
	sys.stdout=RedirectStdout(log_fp)
	sys.stderr=sys.stdout
	nerr=0
	print("Running test suite %s for the KMS transformation library at %s" %(PROG,now))
	print("System is: %s, architecture: %s" %(sys.platform,platform.architecture()))
	print("Python version: %s" %repr(sys.version))
	print("Initialising with shared-library: %s" %(lib))
	ok=TrLib.InitLibrary(geoids,lib_obj,lib_dir)
	if not ok:
		return 1
	print("Files in geoid-dir %s:\n%s" %(TrLib.GEOIDS,repr(os.listdir(TrLib.GEOIDS))))
	print LS
	mlb="utm32_ed50"
	print("Test if we can translate a minilabel:")
	try:
		wkt=TrLib.ExportLabel(mlb,"WKT")
	except:
		nerr+=1
	else:
		if wkt is None or len(wkt)==0:
			print("No recognisable output.")
		else:
			print("%s translates to:\n%s" %(mlb,wkt))
	print LS
	#allow unsafe transformations#
	TrLib.SetThreadMode(False) 
	try:
		nerr+=RandomTests.main(["RandomTests.py","-N",10000])
	except Exception,msg:
		print("Error:\n%s" %repr(msg))
		nerr+=1
	print LS
	try:
		TestFiles.main(["TestFiles.py",JOB_DEF])
	except Exception,msg:
		nerr+=1
		print("Error:\n%s" %repr(msg))
	print LS
	try:
		nerr+=TestProj.main(["TestProj.py"])
	except Exception,msg:
		nerr+=1
		print("Error:\n%s" %repr(msg))
	print LS
	#forbid unsafe transformations#
	if do_threads:
		TrLib.SetThreadMode(True)
		try:
			ThreadTest.main(["ThreadTest.py"])
		except Exception,msg:
			nerr+=1
			print("Error:\n%s" %repr(msg))
	else:
		print("-no_threads specified - will not run thread safety tests....")
	print LS
	print("Tests finished at %s" %time.asctime())
	print("Errors: %i" %nerr)
	return nerr

if __name__=="__main__":
	sys.exit(main(sys.argv))
	
	