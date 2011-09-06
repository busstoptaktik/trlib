using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace Kmstrlib.NET
{
	public static unsafe class Interface
	{
		#region Interface to KMSTRLIB.DLL
		/// <summary>
		/// KMSTRLIB return values:
		/// </summary>
		public enum KMSTR_Error
		{
			KMSTR_OK = 0,  // No Error
			KMSTR_LABELERROR = 1, // Invalid input or output label
			KMSTR_TRANSFORMATIONERROR = 2,  // Transformation failed
			
			
		};

		// Import the relevant functions from the TrLib.dll 
		// It is assumed that this dll (and dependent dll's) are in the
		// same folder as this dll (or in a folder on the system search path)

		[DllImport("TrLib.dll")]
		public static extern int InitLibrary(string folder);

		[DllImport("TrLib.dll")]
		public static extern void TerminateLibrary();

		[DllImport("TrLib.dll")]
		public static extern void* tropen(string mlb1,string mlb2);

        [DllImport("TrLib.dll")]
        public static extern void trclose( void* tr);

		[DllImport("TrLib.dll")]
		public static extern KMSTR_Error tr(
			
			void* TR,
			double* XIn,
			double* YIn,
			double* ZIn,
			int npoints);


		/// <summary>
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
}
