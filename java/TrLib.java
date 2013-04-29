/*
 * Copyright (c) 2013, National Geodata Agency, Denmark
 * (Geodatastyrelsen), gst@gst.dk
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
 
 /* 
* Interface to the TrLib transformation library.
* Modify library name below 
* Include this in your project - provided as is for 'inspiration' - feel free to modify/add methods.
*/

import java.util.*;

public class TrLib {
        //Native method declaration
	public static native String GetVersion();
	public static native int InitLibrary(String folder);
	public static native int GetLastError();
	public static native void TerminateLibrary();
	public static native void TerminateThread();
	public static native void AllowUnsafeTransformations(); //disabled by default....
	public static native void ForbidUnsafeTransformations();
	private static native long Tropen(String mlb1, String mlb2);
	private static native void Trclose(long addr);
	private static native int Transform(long addr, double x[], double y[], double z[], int n);
	
	public static final String JNI_wrap="trlib_java"; //library compiled from the jni interface and linked to the main transformation library trlib (modify name if needed.)
	// Import the relevant functions from the library
	// It is assumed that the libary (and dependent libraries) are in the
	// same folder as this __file__ (or in a folder on the system search path)
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
	public static void SetThreadMode(boolean on){
			if (on)
				ForbidUnsafeTransformations();
			else
				AllowUnsafeTransformations();
		}
	public static class CoordinateTransformation{
		public String mlb_in="";
		public String mlb_out="";
		private long trf=0;
		public CoordinateTransformation(String mlb1, String mlb2) 
		{
			mlb_in=mlb1;
			mlb_out=mlb2;
			trf=TrLib.Tropen(mlb1,mlb2);
			
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
			rc=TrLib.Transform(trf,x,y,z,x.length);
			return TrLib.GetError(rc);
		}
		public TR_Error Transform(double x[], double y[])
		{
			if (trf==0)
				return TR_Error.TR_LABEL_ERROR;
			int rc;
			rc=TrLib.Transform(trf,x,y,null,x.length);
			return TrLib.GetError(rc);
		}	
		public void Close()
		{
			TrLib.Trclose(trf);
	}
}
  
}
 