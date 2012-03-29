"""/*
* Copyright (c) 2011, National Survey and Cadastre, Denmark
* (Kort- og Matrikelstyrelsen), kms@kms.dk
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 */
 """
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
	STD_LIB="KMSTRLIB.dll"
else:
	STD_LIB="KMSTRLIB.so"
STD_DIRNAME=os.path.dirname(__file__)
REQUIRED_FILES=["def_lab.txt","def_shp.txt"]
#define return codes, really define in trlib_api.h
TR_OK=0
LABEL_ERROR=1
TR_ERROR=2
TR_ALLOCATION_ERROR=3
#special return code for this module
TRLIB_NOT_INITIALIZED=-1
#TABDIR env var
TABDIR_ENV="TR_TABDIR"
GEOIDS=None
#conversions
D2R=math.pi/180.0
R2D=180.0/math.pi
#debug flag- turns on extra verbosity here and there
DEBUG=False
#ctypes pointer to double#
LP_c_double=ctypes.POINTER(ctypes.c_double)
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
def InitLibrary(geoid_dir="",lib=STD_LIB,lib_dir=STD_DIRNAME):
	global tr_lib
	global IS_INIT
	global GEOIDS
	
	IS_INIT=False
	if len(lib_dir)==0:
		lib_dir="."
	lib_path=os.path.join(lib_dir,lib)
	if not os.path.exists(lib_path):
		print("%s does not exist!")
		return False
	#to be able to find extra runtime dlls on windows#
	if "win" in sys.platform.lower():
		os.environ["PATH"]+=";"+os.path.realpath(os.path.dirname(lib_path))
	try:
		tr_lib=ctypes.cdll.LoadLibrary(lib_path) #Loads the simple API exported on top of the KMS transformation library.
		#Setup API, corresponds to header file of the API#
		tr_lib.TR_InitLibrary.restype=ctypes.c_int
		tr_lib.TR_InitLibrary.argtypes=[ctypes.c_char_p]
		tr_lib.TR_TerminateThread.restype=None
		tr_lib.TR_TerminateLibrary.restype=None
		tr_lib.TR_GetLastError.argtypes=None
		tr_lib.TR_GetLastError.restype=ctypes.c_int
		tr_lib.TR_GetVersion.argtypes=[ctypes.c_char_p,ctypes.c_int]
		tr_lib.TR_GetVersion.restype=None
		tr_lib.TR_GetEsriText.restype=ctypes.c_int
		tr_lib.TR_GetEsriText.argtypes=[ctypes.c_char_p,ctypes.c_char_p]
		tr_lib.TR_GetLocalGeometry.restype=ctypes.c_int
		tr_lib.TR_GetLocalGeometry.argtypes=[ctypes.c_char_p,ctypes.c_double,ctypes.c_double,LP_c_double,LP_c_double] #todo get type of last arg
		tr_lib.TR_GeoidInfo.argtypes=[ctypes.c_void_p]
		tr_lib.TR_GeoidInfo.restype=None
		tr_lib.TR_Open.restype=ctypes.c_void_p
		tr_lib.TR_Open.argtypes=[ctypes.c_char_p,ctypes.c_char_p,ctypes.c_char_p]
		tr_lib.TR_Close.restype=None
		tr_lib.TR_Close.argtypes=[ctypes.c_void_p]
		tr_lib.TR_Transform.restype=ctypes.c_int
		tr_lib.TR_Transform.argtypes=[ctypes.c_void_p,LP_c_double,LP_c_double,LP_c_double,ctypes.c_int]
		tr_lib.TR_InverseTransform.argtypes=[ctypes.c_void_p,LP_c_double,LP_c_double,LP_c_double,ctypes.c_int]
		tr_lib.TR_InverseTransform.restype=ctypes.c_int
		tr_lib.TR_Transform2.restype=ctypes.c_int
		tr_lib.TR_Transform2.argtypes=[ctypes.c_void_p,LP_c_double,LP_c_double,LP_c_double,LP_c_double,LP_c_double,LP_c_double,ctypes.c_int]
		tr_lib.TR_InverseTransform2.restype=ctypes.c_int
		tr_lib.TR_InverseTransform2.argtypes=[ctypes.c_void_p,LP_c_double,LP_c_double,LP_c_double,LP_c_double,LP_c_double,LP_c_double,ctypes.c_int]
		tr_lib.TR_TransformPoint.restype=ctypes.c_int
		tr_lib.TR_TransformPoint.argtypes=[ctypes.c_void_p,ctypes.c_double,ctypes.c_double,ctypes.c_double,LP_c_double,LP_c_double,LP_c_double]
		tr_lib.TR_InverseTransformPoint.restype=ctypes.c_int
		tr_lib.TR_InverseTransformPoint.argtypes=[ctypes.c_void_p,ctypes.c_double,ctypes.c_double,ctypes.c_double,LP_c_double,LP_c_double,LP_c_double]
		tr_lib.TR_AllowUnsafeTransformations.argtypes=None
		tr_lib.TR_AllowUnsafeTransformations.restype=None
		tr_lib.TR_ForbidUnsafeTransformations.argtypes=None
		tr_lib.TR_ForbidUnsafeTransformations.restype=None
	except Exception, msg:
		print repr(msg)
		print("Unable to load library %s in directory %s." %(lib,lib_dir))
		return False
	if len(geoid_dir)==0:
		if os.environ.has_key(TABDIR_ENV):
			geoid_dir=os.environ[TABDIR_ENV]
		else:
			geoid_dir=os.getcwd()
		sdir=""
	else:
		sdir=geoid_dir
	for fname in REQUIRED_FILES:
		if not os.path.exists(os.path.join(geoid_dir,fname)):
			print("Tabdir: %s" %sdir)
			print("Required file %s not found, failed to initialize library!" %fname)
			return False
	if len(sdir)>0 and sdir[-1] not in ["\\","/"]:
		sdir+="/"
	GEOIDS=geoid_dir
	rc=tr_lib.TR_InitLibrary(sdir) 
	IS_INIT=(rc==TR_OK)
	return IS_INIT

def GetLastError():
	return tr_lib.TR_GetLastError()

def TerminateLibrary():
	tr_lib.TR_TerminateLibrary()

def TerminateThread():
	tr_lib.TR_TerminateThread()

#Must be called if we want to perform transformations which are potentially not thread safe. 
def AllowUnsafeTransformations():
	tr_lib.TR_AllowUnsafeTransformations()

def ForbidUnsafeTransformations():
	tr_lib.TR_ForbidUnsafeTransformations()

#Set multithread mode on/off here,,,
def SetThreadMode(on=True):
	if on:
		ForbidUnsafeTransformations()
	else:
		AllowUnsafeTransformations()


def GetVersion():
	buf=" "*100;
	tr_lib.TR_GetVersion(buf,100)
	ver=buf.replace("\0","").strip()
	return ver

def GetEsriText(label):
	wkt=" "*2048;
	retval=tr_lib.TR_GetEsriText(label,wkt)
	if retval==0:
		wkt=wkt.strip().replace("\0","")
	else:
		wkt=None
	return wkt

def GetLocalGeometry(label,x,y):
	#out param determines wheteher prj_in or prj_out is used#
	mc=ctypes.c_double(0)
	s=ctypes.c_double(0)
	ok=tr_lib.TR_GetLocalGeometry(label,x,y,ctypes.byref(s),ctypes.byref(mc))
	if ok==TR_OK:
		return s.value,(mc.value)*R2D
	return 0,0


def IsGeographic(mlb):
	return ("geo" in mlb[:6])

class CoordinateTransformation(object):
	def __init__(self,mlb_in,mlb_out,geoid_name=""):
		self.mlb_in=mlb_in
		self.mlb_out=mlb_out
		self.geoid_name=geoid_name
		self.tr=None
		self.rc=None #return code object
		self.forward_method=None
		self.inverse_method=None
		self.is_geo_in=False
		self.is_geo_out=False
		if IS_INIT:
			self.tr=tr_lib.TR_Open(mlb_in,mlb_out,geoid_name)
			if self.tr is None:
				raise LabelException()
			self.is_geo_in=IsGeographic(self.mlb_in) #convert2radians?
			self.is_geo_out=IsGeographic(self.mlb_out) #convert2radians?
			self.forward_method=tr_lib.TR_Transform
			self.inverse_method=tr_lib.TR_InverseTransform
		else:
			raise Exception("Initialise Library first!")
	def Transform(self,*args,**kwargs):
		#thus inverse 'switch' must be given as a keyword!#
		if "inverse" in kwargs and kwargs["inverse"]:
			inverse=True
		else:
			inverse=False
		method=None
		if len(args)>1:
			try:
				float(args[0])
			except:
				pass
			else:
				method=self.TransformPoint
		elif len(args)==1:
			if HAS_NUMPY and isinstance(args[0],np.ndarray):
				method=self.TransformArray
			else:
				try: 
					len(args[0])
					len(args[0][0])
				except:
					pass
				else:
					method=self.TransformIterable
		if method is None:
			raise ValueError("Wrong input!")
		return method(*args,inverse=inverse)
	def InverseTransform(self,*args):
		return self.Transform(*args,inverse=True)
	def TransformPoint(self,x,y,z=0.0,inverse=False):
		if self.tr is None:
			raise LabelException()
		if not inverse:
			tr_method=self.forward_method
		else:
			tr_method=self.inverse_method
		if (self.is_geo_in and not inverse) or (self.is_geo_out and inverse):
			x*=D2R
			y*=D2R
		x,y,z=map(ctypes.c_double,[x,y,z])
		res=tr_method(self.tr,ctypes.byref(x),ctypes.byref(y),ctypes.byref(z),1)
		x=x.value
		y=y.value
		z=z.value
		self.rc=res
		if res!=TR_OK:
			raise TransformationException("Error in transformation.")
		if (self.is_geo_out and not inverse) or (self.is_geo_in and inverse):
			x*=R2D
			y*=R2D
		return x,y,z
	#Some copying required here - but overhead *small* compared to transformation
	def TransformArray(self,xyz_in,inverse=False):
		if not HAS_NUMPY:
			raise Exception("This method needs numpy.")
		if self.tr is None:
			raise LabelException()
		if not inverse:
			tr_method=self.forward_method
		else:
			tr_method=self.inverse_method
		if xyz_in.shape[1] not in [2,3]:
			raise Exception("Array column dimension must be 2 or 3!")
		npoints=xyz_in.shape[0]
		if npoints==0:
			return np.empty(xyz_in.shape)
		X=np.require(xyz_in[:,0],dtype=np.float64, requirements=['A', 'O', 'W', 'C'])
		Y=np.require(xyz_in[:,1],dtype=np.float64, requirements=['A', 'O', 'W', 'C'])
		if (self.is_geo_in and not inverse) or (self.is_geo_out and inverse):
			X*=D2R
			Y*=D2R
		if xyz_in.shape[1]==3:  #3d
			Z=np.require(xyz_in[:,2],dtype=np.float64, requirements=['A', 'O', 'W', 'C'])
			res=tr_method(self.tr,X.ctypes.data_as(LP_c_double),Y.ctypes.data_as(LP_c_double),Z.ctypes.data_as(LP_c_double),npoints)
			xyz_out=np.column_stack((X,Y,Z))
		else:
			res=tr_method(self.tr,X.ctypes.data_as(LP_c_double),Y.ctypes.data_as(LP_c_double),None,npoints)
			xyz_out=np.column_stack((X,Y))
		self.rc=res
		#Return values defined in header. Ctypes seems to be unable to import data values from a c-library#
		if res!=TR_OK and not DEBUG:
			if res==TR_ERROR:
				raise TransformationException("Error in transformation.")
			else:
				raise Exception("Allocation/other-error in transformation library.")
		if (self.is_geo_out and not inverse) or (self.is_geo_in and inverse):
			xyz_out[:,0:2]*=R2D
		return xyz_out
	def TransformIterable(self,xyz_in,inverse=False):
		xyz_out=[]
		for data in xyz_in:
			if len(data)==2:
				x,y=data
				x,y,z=self.TransformPoint(x,y,inverse=inverse)
				xyz_out.append([x,y])
			else:
				x,y,z=data
				x,y,z=self.TransformPoint(x,y,z,inverse=inverse)
				xyz_out.append([x,y,z])
		return xyz_out
	def GetReturnCode(self):
		return self.rc
	def GetGeoidInfo(self):
		if self.tr is not None:
			tr_lib.TR_GeoidInfo(self.tr)
	def Close(self):
		if self.tr is not None:
			tr_lib.TR_Close(self.tr)
			self.tr=None
		
	
		
#Transforms a numpy array or a list of coordinates [(x,y,z),.....]
def Transform(label_in,label_out,xyz_in,geoid=""):
	TR=CoordinateTransformation(label_in,label_out,geoid)
	xyz_out=TR.Transform(xyz_in)
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