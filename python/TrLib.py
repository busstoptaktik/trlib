#########################
## Python bindings for simple test api to KmsTrLib/KMSTrans
## simlk, may->sep 2011                
#########################
import numpy as np
import ctypes
import os
import sys
import time
import tempfile
IS_INIT=False
STD_LIB="TrLib"
STD_DIRNAME=os.path.dirname(__file__)
REQUIRED_FILES=["def_lab.txt","def_shp.txt"]
class TransformationException(Exception):
	def __init__(self,msg="Transformation Error"):
		self.msg=msg
	def __str__(self):
		return self.msg
class LabelException(Exception):
	def __init__(self,msg="Label Error"):
		self.msg=msg
	def __str__(self):
		return self.msg
		
##################
##Call this initializtion FIRST!
##################
def InitLibrary(dir,lib=STD_LIB,lib_dir=STD_DIRNAME):
	global tr_lib
	global IS_INIT
	if len(lib_dir)==0:
		lib_dir="."
	try:
		tr_lib=np.ctypeslib.load_library(lib,lib_dir) #Loads the simple API exported on top of all KE's trlib code, see source file KMSTrLib_simle.c
		#Setup API, corresponds to header file of the API#
		tr_lib.InitLibrary.restype=ctypes.c_int
		tr_lib.InitLibrary.argtypes=[ctypes.c_char_p]
		#tr_lib.Transform2D.restype=ctypes.c_int
		#tr_lib.Transform2D.argtypes=[ctypes.c_char_p,ctypes.c_char_p,np.ctypeslib.ndpointer(np.float64,ndim=1,flags='aligned, contiguous,writeable'),
		#np.ctypeslib.ndpointer(np.float64,ndim=1,flags='aligned, contiguous,writeable'),ctypes.c_int]
		tr_lib.GetVersion.argtypes=[ctypes.c_char_p,ctypes.c_int]
		tr_lib.GetVersion.restype=None
		tr_lib.Transform.restype=ctypes.c_int
		tr_lib.Transform.argtypes=[ctypes.c_char_p,ctypes.c_char_p,np.ctypeslib.ndpointer(np.float64,ndim=1,flags='aligned, contiguous,writeable'),
		np.ctypeslib.ndpointer(np.float64,ndim=1,flags='aligned, contiguous,writeable'),ctypes.c_void_p,ctypes.c_int]
		tr_lib.GetEsriText.restype=ctypes.c_int
		tr_lib.GetEsriText.argtypes=[ctypes.c_char_p,ctypes.c_char_p]
	except:
		print("Unable to load library %s in directory %s." %(lib,lib_dir))
		IS_INIT=False
		return False
	if not os.path.exists(dir):
		print("%s does not exist in the file system" %dir)
		IS_INIT=False
		return False
	for fname in REQUIRED_FILES:
		if not os.path.exists(os.path.join(dir,fname)):
			print("Required file %s not found, failed to initialize library!" %fname)
			IS_INIT=False
			return False
	sdir=dir
	if sdir[-1] not in ["\\","/"]:
		sdir+="/"
	tr_lib.InitLibrary(sdir)
	IS_INIT=True
	return True

def GetVersion():
	buf=" "*100;
	tr_lib.GetVersion(buf,100)
	ver=buf.replace("\0","").strip()
	return ver

def GetEsriText(label):
	f_int,tmpname=tempfile.mkstemp()
	ret_val=tr_lib.GetEsriText(label,tmpname)
	#Should be improved with a version that does not depend on reading a file
	#need to make a version of fputshpprj.c which writes to a string - not difficult...
	if ret_val==0:
		f=open(tmpname)
		esri_txt=f.read()
		f.close()
		retval=esri_txt
	else:
		retval=""
	try:
		os.close(f_int)
		os.remove(tmpname)
	except:
		pass
	return retval

def Transform(label_in,label_out,xyz_in):
	if xyz_in.shape[1] not in [2,3]:
		raise Exception("Array column dimension must be 2 or 3!")
		return np.empty((0,2))
	X=np.copy(xyz_in[:,0]).astype(np.float64)
	Y=np.copy(xyz_in[:,1]).astype(np.float64)
	if label_in[0:3]=="geo": #convert2radians?
		if np.fabs(X).max()>2*np.pi:
			X*=np.pi/180.0
			Y*=np.pi/180.0
	npoints=X.shape[0]
	if npoints>0:
		if xyz_in.shape[1]==3:  #3d
			Z=np.copy(xyz_in[:,2]).astype(np.float64)
			res=tr_lib.Transform(label_in,label_out,X,Y,Z.ctypes._data,npoints)
			xyz_out=np.column_stack((X,Y,Z))
		else:
			res=tr_lib.Transform(label_in,label_out,X,Y,None,npoints)
			xyz_out=np.column_stack((X,Y))
		#Return values defined in header. Ctypes seems to be unable to import data values from a c-library#
		if res==2:  
			raise TransformationException("Error in transformation.")
		elif res==1:
			raise LabelException("Input labels are not OK.")
	#print xyz_out.shape
	if label_out[0:3]=="geo": #convert2degrees?
		xyz_out[:,0:2]*=180.0/np.pi
	return xyz_out

##Files to be transformed must follow the format:
## #Label_in        -Remember the hashes in front 
## #Additional comments can be placed below, preceeded by a '#'
## x y (z)
## .....
def TransformFile(file_in,label_out,file_out,precision=8):
	f=open(file_in,"r")
	label_in=(f.readline().strip())[1:]  #skip hash in front
	print("Reading file: %s, input label is: %s" %(file_in,label_in))
	xyz=np.loadtxt(f,comments="#")
	print("Loaded: %i points, dimension is: %i" %(xyz.shape[0],xyz.shape[1]))
	f.close()
	xyz=Transform(label_in,label_out,xyz)
	print("Transformed to: %s" %label_out)
	try:
		f=open(file_out,"w")
	except:
		print("Unable to create outfile: %s" %file_out)
		return
	f.write("#%s\n"%label_out)
	fmt="%"+".%i"%precision+"f"
	np.savetxt(f,xyz,fmt=fmt)
	f.close()

#CALL this method AFTER initialization#
#It is a bit of a mess :-()
def Test(Npoints,repeat=1,f=sys.stdout):
	if not IS_INIT:
		print("Initialize library first!")
	N=Npoints
	f.write("Running benchmark test for %s at %s.\n" %(repr(tr_lib),time.asctime()))
	f.write("Transforming %i points.\n" %N)
	base=np.array([[512300.0,6143200.0]]) #*(np.pi/180.0)
	xy=np.random.rand(N,2)*100.0+base
	f.write("Transformation %s to %s.\n"%("utm32_ed50","geo_wgs84"))
	for i in range(repeat):
		tstart=time.clock()
		out=Transform("utm32_ed50","geo_wgs84",xy)
		f.write("Forward running time: %.5f s\n" %(time.clock()-tstart))
	f.write("Running inverse.\n")
	for i in range(repeat):
		tstart=time.clock()
		end=Transform("geo_wgs84","utm32_ed50",out)
		f.write("Inverse running time: %.5f s\n" %(time.clock()-tstart))
	f.write("Maximum tr-loop error: %.10f m\n" %np.fabs(xy-end).max())
	f.write("Transformation %s to %s.\n"%("utm33_wgs84","utm32_wgs84"))
	for i in range(repeat):
		tstart=time.clock()
		out=Transform("utm33_wgs84","utm32_wgs84",xy)
		f.write("Forward running time: %.5f s\n" %(time.clock()-tstart))
	f.write("Running inverse.\n")
	for i in range(repeat):
		tstart=time.clock()
		end=Transform("utm32_wgs84","utm33_wgs84",out)
		f.write("Inverse running time: %.5f s\n" %(time.clock()-tstart))
	f.write("Maximum tr-loop error: %.10f m\n" %np.fabs(xy-end).max())
		


def Usage():
	print("To run test:")
	print("%s <GEOD_DIR> <TRLIB_library> <#points> <#repetitions>" %os.path.basename(sys.argv[0]))
	sys.exit(1)

def main(args):
	if len(args)<5:
		Usage()
	try:
		N=int(args[3])
		R=int(args[4])
	except:
		Usage()
	OK=InitLibrary(args[1],os.path.basename(args[2]),os.path.dirname(args[2]))
	Test(N,R)
	sys.exit(0)

if __name__=="__main__":
	main(sys.argv)