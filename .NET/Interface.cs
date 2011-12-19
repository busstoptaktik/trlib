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
	public static class Interface
	{
		#region Interface to TrLib.DLL
		/// <summary>
		/// KMSTRLIB return values:
		/// </summary>
		public const string TRLIB="TrLib.dll"; //should really be defined at compile time....
		
		
		public enum KMSTR_Error
		{
			KMSTR_OK = 0,  // No Error
			KMSTR_LABELERROR = 1, // Invalid input or output label
			KMSTR_TRANSFORMATIONERROR = 2,  // Transformation failed
			
			
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
		private static extern void TR_TerminateLibrary();
		
		public static void TerminateLibrary()
		{
			TR_TerminateLibrary();
		}

		[DllImport(TRLIB)]
		public static extern IntPtr tropen(string mlb1,string mlb2, string geoid_name);

		[DllImport(TRLIB)]
		public static extern void trclose( IntPtr tr);

		[DllImport(TRLIB)]
		public static extern KMSTR_Error tr(
			
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
		public static string GetKMSErrorMessage(KMSTR_Error e)
		{
			switch (e)
			{
				case KMSTR_Error.KMSTR_OK: return "No Error";
				case KMSTR_Error.KMSTR_LABELERROR: return "Invalid input or output label";
				case KMSTR_Error.KMSTR_TRANSFORMATIONERROR: return "Transformation failed";
				
			}

			// Unknown error code - throw an exception
			throw new ArgumentException();
		}
		#endregion
	}
	
	public struct Point 
	{
		public double x, y, z;
		public Interface.KMSTR_Error return_code;
		public Point(double p1, double p2, double p3) 
		{
			x = p1;
			y = p2;
			z = p3;
			return_code=Interface.KMSTR_Error.KMSTR_OK;
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
		        TR=Interface.tropen(mlb1,mlb2,"");
			is_init=(TR!=IntPtr.Zero);
			if (!is_init){  //can discuss if we should throw an exception here.....
				throw new ArgumentException("Invalid input labels!");}
		}
		public Point Transform(Point pt)
		{
			Interface.KMSTR_Error err;
			if (!is_init){
				pt.return_code=Interface.KMSTR_Error.KMSTR_LABELERROR;
				return pt;
			}
			err=Interface.tr(TR,out pt.x,out pt.y,out pt.z,1);
			pt.return_code=err;
			return pt;
		}
		public Interface.KMSTR_Error TransformArray(double[] X, double[] Y, double[] Z)
		{
			Interface.KMSTR_Error err=Interface.KMSTR_Error.KMSTR_OK;
			if ((X.Length!=Y.Length)||((Z!=null) && (Z.Length!=X.Length))){
				throw new ArgumentException("Sizes of input arrays must agree!");}
			for (int i=0; i<X.Length ; i++)
				err=Interface.tr(TR,out X[i],out Y[i], out Z[i], 1);
			return err;
		}	
		public void Close()
		{
			Interface.trclose(TR);
		}
			
        }
}
