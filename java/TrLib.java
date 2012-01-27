import java.util.*;

public class TrLib {
        //Native method declaration
	public static native String GetVersion();
	public static native int InitLibrary(String folder);
	public static native int GetLastError();
	public static native void TerminateLibrary();
	public static native void TerminateThread();
	private static native long tropen(String mlb1, String mlb2);
	private static native void trclose(long addr);
	private static native int tr(long addr, double x[], double y[], double z[], int n);
	public static final String JNI_wrap="TrLib_java";
	//Load the library
	static{
		System.loadLibrary(JNI_wrap);
		}
	public static enum TR_Error {
		TR_OK,TR_LABEL_ERROR,TR_TRANSFORMATION_ERROR,TR_ALLOCATION_ERROR,TR_UNKNOWN_ERROR;
		}
	public static TR_Error GetError(int code){ //this could definitely be improved - especially if we implement more return codes!
		switch (code){
			case 0:
				return TR_Error.TR_OK;
			case 1:
				return TR_Error.TR_LABEL_ERROR;
			case 2:
				return TR_Error.TR_TRANSFORMATION_ERROR;
			case 3:
				return TR_Error.TR_ALLOCATION_ERROR;
			default:
				return TR_Error.TR_UNKNOWN_ERROR;
		}
		}
	public static class CoordinateTransformation{
		public String mlb_in="";
		public String mlb_out="";
		private long trf=0;
		public CoordinateTransformation(String mlb1, String mlb2) throws Exception
		{
			mlb_in=mlb1;
			mlb_out=mlb2;
			trf=TrLib.tropen(mlb1,mlb2);
			if (trf==0){
				throw new Exception("Input labels not ok.");}
		}
		public boolean IsInitialised()
		{
			return (trf!=0);
		}
		public TR_Error Transform(double x[], double y[], double z[])
		{
			if (trf==0)
				return TR_Error.TR_LABEL_ERROR;
			int rc;
			rc=TrLib.tr(trf,x,y,z,x.length);
			return TrLib.GetError(rc);
		}
		public TR_Error Transform(double x[], double y[])
		{
			if (trf==0)
				return TR_Error.TR_LABEL_ERROR;
			int rc;
			rc=TrLib.tr(trf,x,y,null,x.length);
			return TrLib.GetError(rc);
		}	
		public void Close()
		{
			TrLib.trclose(trf);
	}
}
  
}
 