#########################
## Python bindings for simple test api to KmsTrLib/KMSTrans
## simlk, may->sep 2011                
#########################
import numpy as np
import ctypes
import os
import sys
import time
IS_INIT=False
STD_LIB="TrLib"
STD_DIRNAME=os.path.dirname(__file__)
REQUIRED_FILES=["def_lab.txt","def_shp.txt"]
#define return codes, really define in trlib_api.h
TR_OK=0
LABEL_ERROR=1
TR_ERROR=2
#special return code for this module
TRLIB_NOT_INITIALIZED=-1
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
def InitLibrary(geoid_dir,lib=STD_LIB,lib_dir=STD_DIRNAME):
	global tr_lib
	global IS_INIT
	IS_INIT=False
	if len(lib_dir)==0:
		lib_dir="."
	try:
		tr_lib=np.ctypeslib.load_library(lib,lib_dir) #Loads the simple API exported on top of all KE's trlib code, see source file KMSTrLib_simle.c
		#Setup API, corresponds to header file of the API#
		tr_lib.InitLibrary.restype=ctypes.c_int
		tr_lib.InitLibrary.argtypes=[ctypes.c_char_p]
		tr_lib.GetTRVersion.argtypes=[ctypes.c_char_p,ctypes.c_int]
		tr_lib.GetTRVersion.restype=None
		tr_lib.Transform.restype=ctypes.c_int
		tr_lib.Transform.argtypes=[ctypes.c_char_p,ctypes.c_char_p,np.ctypeslib.ndpointer(np.float64,ndim=1,flags='aligned, contiguous,writeable'),
		np.ctypeslib.ndpointer(np.float64,ndim=1,flags='aligned, contiguous,writeable'),ctypes.c_void_p,ctypes.c_int]
		tr_lib.GetEsriText.restype=ctypes.c_int
		tr_lib.GetEsriText.argtypes=[ctypes.c_char_p,ctypes.c_char_p]
		tr_lib.tropen.restype=ctypes.c_void_p
		tr_lib.tropen.argtypes=[ctypes.c_char_p,ctypes.c_char_p]
		tr_lib.trclose.restype=None
		tr_lib.trclose.argtypes=[ctypes.c_void_p]
		tr_lib.tr.restype=ctypes.c_int
		tr_lib.tr.argtypes=[ctypes.c_void_p,ctypes.c_void_p,ctypes.c_void_p,ctypes.c_void_p,ctypes.c_int]
	except Exception, msg:
		print repr(msg)
		print("Unable to load library %s in directory %s." %(lib,lib_dir))
		return False
	if not os.path.exists(geoid_dir):
		print("%s does not exist in the file system" %dir)
		return False
	for fname in REQUIRED_FILES:
		if not os.path.exists(os.path.join(geoid_dir,fname)):
			print("Required file %s not found, failed to initialize library!" %fname)
			return False
	sdir=geoid_dir
	if sdir[-1] not in ["\\","/"]:
		sdir+="/"
	has_geoids=tr_lib.InitLibrary(sdir) #at some point use this info....
	IS_INIT=True
	return True

def GetVersion():
	buf=" "*100;
	tr_lib.GetTRVersion(buf,100)
	ver=buf.replace("\0","").strip()
	return ver

def GetEsriText(label):
	wkt=" "*2048;
	retval=tr_lib.GetEsriText(label,wkt)
	if retval==0:
		wkt=wkt.strip().replace("\0","")
	else:
		wkt=None
	return wkt

class CoordinateTransformation(object):
	def __init__(self,mlb_in,mlb_out):
		self.mlb_in=mlb_in
		self.mlb_out=mlb_out
		self.tr=None
		if IS_INIT:
			self.tr=tr_lib.tropen(mlb_in,mlb_out)
			if self.tr is None:
				raise LabelException()
		else:
			raise Exception("Initialise Library first!")
	def Transform(self,x,y,z):
		if self.tr is None:
			raise LabelException()
		x,y,z=map(ctypes.c_double,[x,y,z])
		res=tr_lib.tr(self.tr,ctypes.byref(x),ctypes.byref(y),ctypes.byref(z),1)
		if res==TR_ERROR:
			raise TransformationException("Error in transformation.")
		return x.value,y.value,z.value
	def TransformArray(self,xyz_in):
		if self.tr is None:
			raise LabelException()
		if xyz_in.shape[1] not in [2,3]:
			raise Exception("Array column dimension must be 2 or 3!")
		npoints=xyz_in.shape[0]
		if npoints==0:
			return np.empty(xyz_in.shape)
		X=np.copy(xyz_in[:,0]).astype(np.float64)
		Y=np.copy(xyz_in[:,1]).astype(np.float64)
		if "geo" in label_in[0:6]: #convert2radians?
			if np.fabs(xyz_in[:10,0:2]).max()>2*np.pi: #test a few input coords...
				X*=np.pi/180.0
				Y*=np.pi/180.0
		if xyz_in.shape[1]==3:  #3d
			Z=np.copy(xyz_in[:,2]).astype(np.float64)
			res=tr_lib.tr(self.tr,X,Y,Z.ctypes._data,npoints)
			xyz_out=np.column_stack((X,Y,Z))
		else:
			res=tr_lib.tr(self.tr,X,Y,None,npoints)
			xyz_out=np.column_stack((X,Y))
		#Return values defined in header. Ctypes seems to be unable to import data values from a c-library#
		if res!=TR_OK:
			if res==TR_ERROR:  
				raise TransformationException("Error in transformation.")
			else:
				raise Exception("Unknown return code from transformation library.")
		#print xyz_out.shape
		if "geo" in label_out[0:6]: #convert2degrees?
			xyz_out[:,0:2]*=180.0/np.pi
		return xyz_out
	def Close(self):
		if self.tr is not None:
			tr_lib.trclose(self.tr)
		
	
		

def Transform(label_in,label_out,xyz_in):
	if xyz_in.shape[1] not in [2,3]:
		raise Exception("Array column dimension must be 2 or 3!")
	npoints=xyz_in.shape[0]
	if npoints==0:
		return np.empty(xyz_in.shape)
	X=np.copy(xyz_in[:,0]).astype(np.float64)
	Y=np.copy(xyz_in[:,1]).astype(np.float64)
	if "geo" in label_in[0:6]: #convert2radians?
		if np.fabs(xyz_in[:10,0:2]).max()>2*np.pi: #test a few input coords...
			X*=np.pi/180.0
			Y*=np.pi/180.0
	if xyz_in.shape[1]==3:  #3d
		Z=np.copy(xyz_in[:,2]).astype(np.float64)
		res=tr_lib.Transform(label_in,label_out,X,Y,Z.ctypes._data,npoints)
		xyz_out=np.column_stack((X,Y,Z))
	else:
		res=tr_lib.Transform(label_in,label_out,X,Y,None,npoints)
		xyz_out=np.column_stack((X,Y))
	#Return values defined in header. Ctypes seems to be unable to import data values from a c-library#
	if res!=0:
		if res==2:  
			raise TransformationException("Error in transformation.")
		elif res==1:
			raise LabelException("Input labels are not OK.")
		else:
			raise Exception("Unknown return code from transformation library.")
	#print xyz_out.shape
	if "geo" in label_out[0:6]: #convert2degrees?
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