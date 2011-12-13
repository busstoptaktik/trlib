import java.util.*;

public class TrLib {
        //Native method declaration
	public static native String GetVersion();
	public static native int InitLibrary(String folder);
	public static native void TerminateLibrary();
	private static native long tropen(String mlb1, String mlb2);
	private static native void trclose(long addr);
	protected static native int tr(long addr, double x[], double y[], double z[], int n);
	public static final String JNI_wrap="TrLib_java";
	public static enum TR_error{
		TR_OK (0),
		TR_LABEL_ERROR (1),
		TR_TRANSFORMATION_ERROR (2);
		public int error;
		TR_error( int error)
		{
			this.error=error;
		}
		
	}
	//Load the library
	static{
		System.loadLibrary(JNI_wrap);
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
			throw new Exception("Input labels nok ok.");}
		System.out.format("addr: %d%n",trf);
		
	}
	public boolean IsInitialised()
	{
		return (trf!=0);
	}
	public int Transform(double x[], double y[], double z[]) throws Exception
	{
		int rc;
		if (trf==0){
			throw new Exception("Not properly initialised");}
		rc=TrLib.tr(trf,x,y,z,x.length);
		return rc;
	}			
	public void Close()
	{
		TrLib.trclose(trf);
	}
}
  
}
 