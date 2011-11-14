#########################
## Python bindings for simple test api to KmsTrLib/KMSTrans
## simlk, may->sep 2011                
#########################
try:
	import numpy as np
except:
	HAS_NUMPY=False
else:
	HAS_NUMPY=True
import math
import ctypes
import os
import sys
import time
IS_INIT=False
if "win" in sys.platform:
	STD_LIB="TrLib.dll"
else:
	STD_LIB="TrLib.so"
STD_DIRNAME=os.path.dirname(__file__)
REQUIRED_FILES=["def_lab.txt","def_shp.txt"]
#define return codes, really define in trlib_api.h
TR_OK=0
LABEL_ERROR=1
TR_ERROR=2
#special return code for this module
TRLIB_NOT_INITIALIZED=-1
#conversions
D2R=math.pi/180.0
R2D=180.0/math.pi
#debug flag- turns on extra verbosity here and there
DEBUG=False
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
		tr_lib=ctypes.cdll.LoadLibrary(os.path.join(lib_dir,lib)) #Loads the simple API exported on top of the KMS transformation library.
		#Setup API, corresponds to header file of the API#
		tr_lib.InitLibrary.restype=ctypes.c_int
		tr_lib.InitLibrary.argtypes=[ctypes.c_char_p]
		tr_lib.TR_GetLastError.argtypes=None
		tr_lib.TR_GetLastError.restype=ctypes.c_int
		tr_lib.GetTRVersion.argtypes=[ctypes.c_char_p,ctypes.c_int]
		tr_lib.GetTRVersion.restype=None
		tr_lib.GetEsriText.restype=ctypes.c_int
		tr_lib.GetEsriText.argtypes=[ctypes.c_char_p,ctypes.c_char_p]
		tr_lib.tropen.restype=ctypes.c_void_p
		tr_lib.tropen.argtypes=[ctypes.c_char_p,ctypes.c_char_p,ctypes.c_char_p]
		tr_lib.trclose.restype=None
		tr_lib.trclose.argtypes=[ctypes.c_void_p]
		tr_lib.tr.restype=ctypes.c_int
		tr_lib.tr.argtypes=[ctypes.c_void_p,ctypes.c_void_p,ctypes.c_void_p,ctypes.c_void_p,ctypes.c_int]
	except Exception, msg:
		print repr(msg)
		print("Unable to load library %s in directory %s." %(lib,lib_dir))
		return False
	if not os.path.exists(geoid_dir):
		print("%s does not exist in the file system" %geoid_dir)
		return False
	for fname in REQUIRED_FILES:
		if not os.path.exists(os.path.join(geoid_dir,fname)):
			print("Required file %s not found, failed to initialize library!" %fname)
			return False
	sdir=geoid_dir
	if sdir[-1] not in ["\\","/"]:
		sdir+="/"
	IS_INIT=tr_lib.InitLibrary(sdir) #at some point use this info....
	return IS_INIT

def GetLastError():
	return tr_lib.TR_GetLastError()

def TerminateLibrary():
	tr_lib.TerminateLibrary()

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
	def __init__(self,mlb_in,mlb_out,geoid_name=""):
		self.mlb_in=mlb_in
		self.mlb_out=mlb_out
		self.geoid_name=geoid_name
		self.tr=None
		self.rc=None #return code object
		if IS_INIT:
			self.tr=tr_lib.tropen(mlb_in,mlb_out,geoid_name)
			if self.tr is None:
				raise LabelException()
		else:
			raise Exception("Initialise Library first!")
	def Transform(self,x,y,z=0.0):
		if self.tr is None:
			raise LabelException()
		if "geo" in self.mlb_in[0:6]: #convert2radians?
			x*=D2R
			y*=D2R
		x,y,z=map(ctypes.c_double,[x,y,z])
		res=tr_lib.tr(self.tr,ctypes.byref(x),ctypes.byref(y),ctypes.byref(z),1)
		x=x.value
		y=y.value
		z=z.value
		self.rc=res
		if res==TR_ERROR:
			raise TransformationException("Error in transformation.")
		if "geo" in self.mlb_out[0:6]:
			x*=R2D
			y*=R2D
		return x,y,z
	def TransformArray(self,xyz_in):
		if not HAS_NUMPY:
			raise Exception("This method needs numpy.")
		if self.tr is None:
			raise LabelException()
		if xyz_in.shape[1] not in [2,3]:
			raise Exception("Array column dimension must be 2 or 3!")
		npoints=xyz_in.shape[0]
		if npoints==0:
			return np.empty(xyz_in.shape)
		X=np.require(xyz_in[:,0],dtype=np.float64, requirements=['A', 'O', 'W', 'C'])
		Y=np.require(xyz_in[:,1],dtype=np.float64, requirements=['A', 'O', 'W', 'C'])
		if "geo" in self.mlb_in[0:6]: #convert2radians?
			X*=D2R
			Y*=D2R
		if xyz_in.shape[1]==3:  #3d
			Z=np.require(xyz_in[:,2],dtype=np.float64, requirements=['A', 'O', 'W', 'C'])
			res=tr_lib.tr(self.tr,X.ctypes._data,Y.ctypes._data,Z.ctypes._data,npoints)
			xyz_out=np.column_stack((X,Y,Z))
		else:
			res=tr_lib.tr(self.tr,X.ctypes._data,Y.ctypes._data,None,npoints)
			xyz_out=np.column_stack((X,Y))
		self.rc=res
		#Return values defined in header. Ctypes seems to be unable to import data values from a c-library#
		if res!=TR_OK and not DEBUG:
			if res==TR_ERROR:
				raise TransformationException("Error in transformation.")
			else:
				raise Exception("Unknown return code from transformation library.")
		if "geo" in self.mlb_out[0:6]: #convert2degrees?
			xyz_out[:,0:2]*=R2D
		return xyz_out
	def GetReturnCode(self):
		return self.rc
	def Close(self):
		if self.tr is not None:
			tr_lib.trclose(self.tr)
		
	
		

def Transform(label_in,label_out,xyz_in,geoid=""):
	TR=CoordinateTransformation(label_in,label_out,geoid)
	if HAS_NUMPY and isinstance(xyz_in,np.ndarray):
		xyz_out=TR.TransformArray(xyz_in)
	else:
		dim=len(xyz_in[0])
		xyz_out=[]
		for data in xyz_in:
			if len(data)==2:
				x,y=data
				x,y,z=TR.Transform(x,y)
				xyz_out.append([x,y])
			else:
				x,y,z=data
				x,y,z=TR.Transform(x,y,z)
				xyz_out.append([x,y,z])
	if DEBUG:
		rc=TR.GetReturnCode()
		print("rc: %d" %rc)
	TR.Close()
	return xyz_out
##Files to be transformed must follow the format:
## #Label_in        -Remember the hashes in front 
## #Additional comments can be placed below, preceeded by a '#'
## x y (z)
## .....
def TransformFile(file_in,label_out,file_out,precision=8):
	f=open(file_in,"r")
	print("Reading file: %s" %file_in)
	xyz=[]
	label_in=None
	line=f.readline()
	while len(line)>0:
		sline=line.split()
		if len(sline)==0:
			line=f.readline()
			continue
		if sline[0][0]=="#" and label_in is None:
			label_in=sline[0][1:]
		elif len(sline)==2:
			try:
				x=float(sline[0])
				y=float(sline[1])
			except:
				pass
			else:
				xyz.append([x,y])
		elif len(sline)>2:
			try:
				x=float(sline[0])
				y=float(sline[1])
				z=float(sline[2])
			except:
				pass
			else:
				xyz.append([x,y,z])
		line=f.readline()	
	npoints=len(xyz)
	dim=None
	if npoints>0:
		dim=len(xyz[0])
	else:
		print("No points!")
		return -1
	print("Loaded: %i points, dimension is: %i" %(npoints,dim))
	f.close()
	if HAS_NUMPY:
		xyz=np.array(xyz,dtype=np.float64)
	xyz=Transform(label_in,label_out,xyz)
	print("Transformed to: %s" %label_out)
	try:
		f=open(file_out,"w")
	except:
		print("Unable to create outfile: %s" %file_out)
		return
	print("Writing %s" %file_out)
	f.write("#%s\n"%label_out)
	fmt="%"+".%i"%precision+"f"
	dim=len(xyz[0])
	line_fmt=""
	for i in range(dim):
		line_fmt+="%s "%fmt
	line_fmt=line_fmt[:-1]+"\n"
	for row in xyz:
		f.write(line_fmt %tuple(row))
	f.close()

#CALL this method AFTER initialization#
#It is a bit of a mess :-()
def Test(Npoints,repeat=1,f=sys.stdout):
	if not IS_INIT:
		print("Initialize library first!")
		return -1
	if not HAS_NUMPY:
		print("Needs numpy!")
		return -1
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