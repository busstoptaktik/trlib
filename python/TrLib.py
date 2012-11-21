"""/*
* Copyright (c) 2012, National Survey and Cadastre, Denmark
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
## Python bindings for (Kms) TrLib
## simlk, 2011,2012            
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
if sys.platform.startswith("win"):
	STD_LIB="KMSTRLIB.dll"
elif "darwin" in sys.platform:
	STD_LIB="KMSTRLIB.dylib"
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
#Globals for minilabel parsing
SEPS=["E","H","N"]
#debug flag- turns on extra verbosity here and there
DEBUG=False
#ctypes pointer to double#
LP_c_double=ctypes.POINTER(ctypes.c_double)
LP_c_int=ctypes.POINTER(ctypes.c_int)
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
		
###################
##Call this initializtion FIRST!
###################
#TODO: change prints to raise some Exception
#Or: stdout must be mapped to some other logging method....
def InitLibrary(geoid_dir="",lib=STD_LIB,lib_dir=STD_DIRNAME):
	global tr_lib
	global IS_INIT
	global GEOIDS
	
	IS_INIT=False
	if len(lib_dir)==0:
		lib_dir="."
	lib_path=os.path.join(lib_dir,lib)
	if not os.path.exists(lib_path):
		print("%s does not exist!" %lib_path)
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
		tr_lib.TR_GetLocalGeometry.argtypes=[ctypes.c_void_p,ctypes.c_double,ctypes.c_double,LP_c_double,LP_c_double] #todo get type of last arg
		tr_lib.TR_GeoidInfo.argtypes=[ctypes.c_void_p]
		tr_lib.TR_GeoidInfo.restype=None
		tr_lib.TR_GetGeoidName.argtypes=[ctypes.c_void_p,ctypes.c_char_p]
		tr_lib.TR_GetGeoidName.restype=None
		tr_lib.TR_Open.restype=ctypes.c_void_p
		tr_lib.TR_Open.argtypes=[ctypes.c_char_p,ctypes.c_char_p,ctypes.c_char_p]
		tr_lib.TR_Compose.restype=ctypes.c_void_p
		tr_lib.TR_Compose.argtypes=[ctypes.c_void_p,ctypes.c_void_p]
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
		#Some extra functions which might get exposed in the API#
		tr_lib.TR_OpenProjection.argtypes=[ctypes.c_char_p]
		tr_lib.TR_OpenProjection.restype=ctypes.c_void_p
		tr_lib.TR_SetGeoidDir.argtypes=[ctypes.c_char_p]
		tr_lib.TR_SetGeoidDir.restype=ctypes.c_int
		tr_lib.set_grs.argtypes=[ctypes.c_int,ctypes.c_char_p,LP_c_double]
		tr_lib.bshlm1.restype=ctypes.c_int
		tr_lib.bshlm1.argtypes=[ctypes.c_double]*3+[LP_c_double,ctypes.c_double]*3
		tr_lib.bshlm2.restype=ctypes.c_int
		tr_lib.bshlm2.argtypes=[ctypes.c_double]*6+[LP_c_double]*3+[ctypes.c_int]
		tr_lib.doc_prj.argtypes=[ctypes.c_char_p]*3+[LP_c_int]+[ctypes.c_int]
		tr_lib.doc_prj.restype=ctypes.c_int
		tr_lib.doc_dtm.argtypes=[ctypes.c_char_p]*2+[ctypes.c_int]
		tr_lib.doc_dtm.restype=ctypes.c_int
		tr_lib.sgetshpprj.argtypes=[ctypes.c_char_p,ctypes.c_void_p,ctypes.c_char_p]
		tr_lib.sgetshpprj.restype=ctypes.c_int
		tr_lib.proj4_to_mlb.argtypes=[ctypes.c_char_p]*2
		tr_lib.proj4_to_mlb.restype=ctypes.c_int
		tr_lib.set_lord_file.argtypes=[ctypes.c_char_p]
		tr_lib.set_lord_file.restype=None

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

#Convenience method for getting a proper python string out of a string returned from c-function
def GetString(cstring):
	i=cstring.find("\0")
	if (i!=-1):
		return cstring[:i].strip()
	return cstring.strip()

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

def SetGeoidDir(geoid_dir):
	global GEOIDS
	msg=""
	for fname in REQUIRED_FILES:
		if not os.path.exists(os.path.join(geoid_dir,fname)):
			msg+="Required file %s not found.\n" %fname
			return False,msg
	if geoid_dir[-1] not in ["\\","/"]:
		geoid_dir+="/"
	ok=tr_lib.TR_SetGeoidDir(geoid_dir)
	if (ok!=TR_OK):
		msg+="Failed to initialise library - probably a bad def_lab file."
	else:
		GEOIDS=geoid_dir
	return ok==TR_OK,msg


def GetVersion():
	buf=" "*100;
	tr_lib.TR_GetVersion(buf,100)
	ver=buf.replace("\0","").strip()
	return ver

#######################
## Minilabel conversion methods 
#######################
def GetEsriText(label):
	if IS_INIT:
		wkt=" "*2048;
		retval=tr_lib.TR_GetEsriText(label,wkt)
		if retval==0:
			return GetString(wkt)
	return None
	
def FromEsriText(wkt):
	if IS_INIT:
		out=" "*256;
		retval=tr_lib.sgetshpprj(wkt,None,out)
		if retval==0:
			return GetString(out)
		elif DEBUG:
			print retval
	return None

def FromProj4(proj4def):
	mlb=" "*256
	retval=tr_lib.proj4_to_mlb(proj4def,mlb)
	if retval!=TR_OK:
		return None
	else:
		return GetString(mlb)

###########################
## Geometry analysis methods
###########################


def BesselHelmert(axis,flat,lon1,lat1,lon2,lat2):
	if IS_INIT:
		if (max(abs(lon1-lon2),abs(lat1-lat2))<1e-11):
			return 0.0,90.0,0.0
		d=ctypes.c_double(0)
		a1=ctypes.c_double(0)
		a2=ctypes.c_double(0)
		ret=tr_lib.bshlm2(axis,flat,lat1*D2R,lat2*D2R,lon1*D2R,lon2*D2R,ctypes.byref(a1),ctypes.byref(a2),ctypes.byref(d),0)
		if ret<2:
			return d.value,a1.value*R2D,a2.value*R2D
	return None,None,None

def InverseBesselHelmert(axis,flat,lon1,lat1,a1,dist):
	if IS_INIT:
		a2=ctypes.c_double(0)
		lon2=ctypes.c_double(0)
		lat2=ctypes.c_double(0)
		ret=tr_lib.bshlm1(axis,flat,lat1*D2R,ctypes.byref(lat2),lon1*D2R,ctypes.byref(lon2),a1*D2R,ctypes.byref(a2),dist)
		if (ret==0 or ret==1):
			return lon2.value*R2D,lat2.value*R2D,a2.value*R2D
	return None,None,None

#############################
## 'Documentation' methods
#############################

def GetEllipsoidParameters(ell_name):
        if IS_INIT:
                arr=ctypes.c_double*9
                ell_par=arr()
                ret=tr_lib.set_grs(-1,ell_name,ctypes.cast(ell_par,LP_c_double))
                if ret>0:
                        return ell_par[0],ell_par[1]
        return None,None

def GetEllipsoidParametersFromDatum(mlb_dtm):
	if IS_INIT:
		ell_name=" "*128
		rc=tr_lib.doc_dtm(mlb_dtm,ell_name,-1)
		if (rc==TR_OK):
			a,f=GetEllipsoidParameters(ell_name)
			return GetString(ell_name),a,f
	return None,None,None
		

def DescribeProjection(mlb_prj):
	if IS_INIT:
		descr=" "*512
		impl_dtm=" "*64
		type=ctypes.c_int(0)
		rc=tr_lib.doc_prj(mlb_prj,descr,impl_dtm,ctypes.byref(type),0)
		if (rc==TR_OK):
			if ("(" in descr):
				descr=descr[:descr.find("(")]
			return GetString(descr),GetString(impl_dtm)
	return None,None

def DescribeDatum(mlb_dtm):
	if IS_INIT:
		if (mlb_dtm=="E"):
			return "Ellipsoidal heights"
		if (mlb_dtm=="N"):
			mlb_dtm="Normal heights"
		descr=" "*512
		rc=tr_lib.doc_dtm(mlb_dtm,descr,0)
		if (rc==TR_OK):
			return GetString(descr).replace("@","")
	return None


def DescribeLabel(mlb):
	try:
		region,prj,dtm,h_dtm,h_type=SplitMLB(mlb)
	except Exception,msg:
		return "Bad minilabel: "+repr(msg)
	if (not IS_INIT):
		return "Library not initialised."
	descr_prj,impl_dtm=DescribeProjection(prj)
	if descr_prj is None:
		descr_prj="Projection not ok"
	elif (len(impl_dtm)>0 and len(dtm)==0):
		dtm=impl_dtm
	descr_dtm=DescribeDatum(dtm)
	if (descr_dtm is None):
		descr_dtm="datum not ok"
	if (len(h_dtm)>0):
		descr_h_dtm=DescribeDatum(h_dtm)
		if (descr_h_dtm is None):
			descr_h_dtm="height datum not ok"
	else:
		descr_h_dtm=""
	descr=descr_prj+", "+descr_dtm
	if len(descr_h_dtm)>0:
		descr+=", "+descr_h_dtm
	return descr+"."


######################
## Minilabel analysis methods 
######################
def SplitMLB(mlb):
	mlb=mlb.split()[0].replace("L","_")
	region=""
	proj=""
	datum=""
	hdatum=""
	htype=""
	sep=""
	if len(mlb)>2:
		if mlb[:2].isupper() and mlb[2]=="_":
			region=mlb[:2]
			mlb=mlb[3:]
	if len(mlb)>1:
		for ssep in SEPS:
			if ssep in mlb:
				sep=ssep
				break
		#no heights#
		if sep=="":
			if "_" in mlb:
				proj,datum=mlb.split("_")
			else:
				proj=mlb
		#heights#
		else:
			proj,datum=mlb.split(sep)
			if "_" in datum:
				if sep!="H":
					raise Exception("Bad minilabel")
				datum,htype,hdatum=datum.split("_")
			else:
				hdatum=sep
	return region,proj,datum,hdatum,htype

def IsGeographic(mlb):
	try:
		region,proj,datum,hdatum,htype=SplitMLB(mlb)
	except:
		return False
	return proj=="geo"

#Will throw an exception if datum is implicit and library is not initialized
def GetDatum(mlb):
	region,prj,dtm,h_dtm,h_type=SplitMLB(mlb)
	if len(dtm)>0:
		return dtm
	if (not IS_INIT):
		raise Exception("Initliaze library first!")
	descr_prj,impl_dtm=DescribeProjection(prj)
	if descr_prj is None or len(impl_dtm)==0:
		raise LabelException("Label not OK!")
	return impl_dtm

#Will throw an exception if datum is implicit and library is not initialized
#Converts mlb to 2d-geographic projection based on same datum
def Convert2Geo(mlb):
	region,prj,dtm,h_dtm,h_type=SplitMLB(mlb)
	if len(dtm)>0:
		dtm=GetDatum(mlb)
	return "geo_"+dtm


################################
## Projection Class
################################
class CoordinateSystem(object):
	def __init__(self,mlb):
		if not IS_INIT:
			raise Exception("Initialise Library first!")
		self.mlb=mlb
		self.tr=tr_lib.TR_Open(mlb,"","")
		if self.tr is None:
			raise LabelException()
		self.is_geo=IsGeographic(mlb)
	#Destructor - if we have forgotten to close#
	def __del__(self):
		self.Close()	
	def GetLocalGeometry(self,x,y):
		#out param determines wheteher prj_in or prj_out is used#
		if self.tr is None:
			raise Exception("This system has been invalidated!")
		if self.is_geo:
			return 1,0
		mc=ctypes.c_double(0)
		s=ctypes.c_double(0)
		ok=tr_lib.TR_GetLocalGeometry(self.tr,x,y,ctypes.byref(s),ctypes.byref(mc))
		if ok==TR_OK:
			return s.value,(mc.value)*R2D
		return 0,0
	def Close(self):
		if self.tr is not None:
			tr_lib.TR_Close(self.tr)
		self.tr=None
		

################################
## Main Transformation Class
################################


class CoordinateTransformation(object):
	"""Main transformation object. 
	The constructer accepts string input as well as input of type CoordinateSystem.
	A CoordinateSystem used as input will be invalidated by the constructer. 
	Pointers to structures in the underlying c-library are now owned by the CoordinateTransformation class.
	"""
	def __init__(self,system_in,system_out,geoid_name=""):
		if not IS_INIT:
			raise Exception("Initialise Library first!")
		self.tr=None
		_systems=[]
		for system in [system_in,system_out]:
			if isinstance(system,CoordinateSystem):
				_systems.append(system)
				
			elif isinstance(system,str):
				_systems.append(CoordinateSystem(system))
				
			else: 
				raise ValueError("Wrong type of input system.")
		self.tr=tr_lib.TR_Compose(_systems[0].tr,_systems[1].tr)
		if self.tr is None:
			raise LabelException("Failed to compose transformation.")
		self.mlb_in=_systems[0].mlb
		self.mlb_out=_systems[1].mlb
		self.geoid_name=geoid_name
		self.rc=None #return code object
		self.forward_method=tr_lib.TR_Transform
		self.inverse_method=tr_lib.TR_InverseTransform
		self.is_geo_in=IsGeographic(self.mlb_in) #convert2radians?
		self.is_geo_out=IsGeographic(self.mlb_out) #convert2radians?
	#Destructor - if we have forgotten to close#
	def __del__(self):
		self.Close()	
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
			raise Exception("This transformation has been invalidated")
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
			raise Exception("This transformation has been invalidated")
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
	def GetGeoidName(self):
		if self.tr is not None:
			name=" "*256
			tr_lib.TR_GetGeoidName(self.tr,name)
			return name.replace("\0","").strip()
		return ""
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