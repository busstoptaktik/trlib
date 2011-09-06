using System;
using System.Collections.Generic;
using System.Text;

namespace Kmstrlib.NET
{
	unsafe class TestProgram
	{
		static void Main(string[] args)
		{
			Interface.KMSTR_Error  error;
            int err;
            void* tr;
			err = Interface.InitLibrary("C:\\Geoids\\");
            System.Console.WriteLine("Init: {0}\n", err);
            double X = 400000;
			double Y = 6500000;
			double Z = 0;
            System.Console.WriteLine("in:  X: {0,8:f}  Y: {1,8:f} Z: {2,4:f}\n", X, Y, Z);
            tr=Interface.tropen("utm32Eed50","utm32Ewgs84");
			error =  Interface.tr(tr, &X, &Y, &Z, 1);
            if (error != Interface.KMSTR_Error.KMSTR_OK) throw new Exception(Interface.GetKMSErrorMessage(error));
            System.Console.WriteLine("out: X: {0,8:f}  Y: {1,8:f} Z: {2,4:f} ret: {3,0:d}\n", X, Y, Z,error);
            Interface.trclose(tr);
		}
	}
}
