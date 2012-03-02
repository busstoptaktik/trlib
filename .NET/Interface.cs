/*
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

/* 
* Interface to the TrLib transformation library.
* Modify library name below 
*/

using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
namespace Kmstrlib.NET
{
	public static class TrLib
	{
		#region Interface to TrLib.DLL
		/// <summary>
		/// KMSTRLIB return values:
		/// </summary>
		public const string TRLIB="TrLib.dll"; //should really be defined at compile time....
		
		
		public enum TR_Error
		{
			TR_OK = 0,  // No Error
			TR_LABEL_ERROR = 1, // Invalid input or output label
			TR_TRANSFORMATION_ERROR = 2,  // Transformation failed
			TR_ALLOCATION_ERROR=3 //Memory allocation failed
			
			
		};

		// Import the relevant functions from the TrLib.dll 
		// It is assumed that this dll (and dependent dll's) are in the
		// same folder as this __file__ (or in a folder on the system search path)

		[DllImport(TRLIB)]
		private static extern int TR_InitLibrary(string folder);
		
		public static int InitLibrary(string folder)
		{
			return TR_InitLibrary(folder);
		}
		[DllImport(TRLIB)]
		private static extern int TR_GetLastError();
		
		public static int GetLastError()
		{
			return TR_GetLastError();
		}

		[DllImport(TRLIB)]
		private static extern void TR_TerminateLibrary();
		
		[DllImport(TRLIB)]
		private static extern void TR_TerminateThread();
		
		[DllImport(TRLIB)]
		private static extern void TR_AllowUnsafeTransformations();
		
		[DllImport(TRLIB)]
		private static extern void TR_ForbidUnsafeTransformations();
		
		public static void AllowUnsafeTransformations(){
			TR_AllowUnsafeTransformations();
		}
		
		public static void ForbidUnsafeTransformations(){
			TR_ForbidUnsafeTransformations();
		}
		
		public static void SetThreadMode(bool on){
			if (on)
				ForbidUnsafeTransformations();
			else
				AllowUnsafeTransformations();
		}
		
		public static void TerminateLibrary()
		{
			TR_TerminateLibrary();
		}
		
		public static void TerminateThread()
		{
			TR_TerminateThread();
		}

		[DllImport(TRLIB)]
		public static extern IntPtr TR_Open(string mlb1,string mlb2, string geoid_name);

		[DllImport(TRLIB)]
		public static extern void TR_Close( IntPtr tr);

		[DllImport(TRLIB)]
		public static extern TR_Error TR_Transform(
			
			IntPtr TR,
			out double X,
			out double Y,
			out double Z,
			int npoints);
		
		[DllImport(TRLIB)]
		public static extern void TR_GetVersion(StringBuilder buf, int buf_length);
		
		
		
		public static string GetVersion()
		{
			StringBuilder cb = new StringBuilder(256);
			TR_GetVersion(cb,cb.Capacity);
			return cb.ToString().Trim().Replace("\0",""); 
			
		}
		
              
		/// String representation of error messages
		/// </summary>
		public static string GetKMSErrorMessage(TR_Error e)
		{
			switch (e)
			{
				case TR_Error.TR_OK: return "No Error";
				case TR_Error.TR_LABEL_ERROR: return "Invalid input or output label";
				case TR_Error.TR_TRANSFORMATION_ERROR: return "Transformation failed";
				case TR_Error.TR_ALLOCATION_ERROR: return "Allocation failed";
				
			}

			// Unknown error code - throw an exception
			throw new ArgumentException();
		}
		#endregion
	}
	
	public class Point 
	{
		public double x, y, z;
		public TrLib.TR_Error return_code;
		public Point(double p1, double p2, double p3) 
		{
			x = p1;
			y = p2;
			z = p3;
			return_code=TrLib.TR_Error.TR_OK;
		}
	}
	
	
	public class CoordinateTransformation
	{
		public string mlb_in;
		public string mlb_out;
		public bool is_init=false;
		private IntPtr TR=IntPtr.Zero;
		public CoordinateTransformation(string mlb1, string mlb2)
		{
			mlb_in=mlb1;
			mlb_out=mlb2;
		        TR=TrLib.TR_Open(mlb1,mlb2,"");
			is_init=(TR!=IntPtr.Zero);
			if (!is_init){  //can discuss if we should throw an exception here.....
				throw new ArgumentException("Invalid input labels!");}
		}
		/* versions of overloaded transformation method defined here */
		public TrLib.TR_Error Transform(Point pt)
		{
			TrLib.TR_Error err;
			if (!is_init){
				pt.return_code=TrLib.TR_Error.TR_LABEL_ERROR;
				return TrLib.TR_Error.TR_LABEL_ERROR;
			}
			err=TrLib.TR_Transform(TR,out pt.x,out pt.y,out pt.z,1);
			pt.return_code=err;
			return (TrLib.TR_Error) err;
		}
		public TrLib.TR_Error Transform(double[] X, double[] Y, double[] Z)
		{
			TrLib.TR_Error err=TrLib.TR_Error.TR_OK;
			if ((X.Length!=Y.Length)||(Z.Length!=X.Length)){
				throw new ArgumentException("Sizes of input arrays must agree!");}
			for (int i=0; i<X.Length ; i++){
				err=TrLib.TR_Transform(TR,out X[i],out Y[i], out Z[i], 1);
			}
			return (TrLib.TR_Error) err;
		}
		public TrLib.TR_Error Transform(double[] X, double[] Y)
		{
			TrLib.TR_Error err=TrLib.TR_Error.TR_OK;
			double z;
			if (X.Length!=Y.Length)
				throw new ArgumentException("Sizes of input arrays must agree!");
			for (int i=0; i<X.Length ; i++){
				z=0;
				err=TrLib.TR_Transform(TR,out X[i],out Y[i], out z, 1);
			}
			return (TrLib.TR_Error) err;
		}
      		public TrLib.TR_Error Transform(ref double x, ref double y, ref double z){
			TrLib.TR_Error err=TrLib.TR_Error.TR_OK;
			err=TrLib.TR_Transform(TR, out x, out y, out z,1);
			return (TrLib.TR_Error) err;
		}
		public void Close()
		{
			TrLib.TR_Close(TR);
		}
			
        }
}
