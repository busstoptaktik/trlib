using System;
using System.Collections.Generic;
using System.Text;

namespace Kmstrlib.NET
{
	class TestProgram
	{
		static void Main(string[] args)
		{
			Interface.KMSTR_Error  error;
			int i,err;
			if (args.Length>0){
				System.Console.WriteLine("Initialising library.");
				err = Interface.InitLibrary(args[0]);
				System.Console.WriteLine("Init: {0}\n", err);}
			else{
				System.Console.WriteLine("Specify geoid library as first input parameter!");
				return;}
			System.Console.WriteLine("Loading functions from {0}",Interface.TRLIB);
			string version=Interface.GetVersion();
			System.Console.WriteLine("This is TrLib. v. {0}\n",version);
			double[] X =new double[] {512200.1,512499.1};
			double[] Y =new double[] {6143200.1,6143300.1};
			double[] Z =new double[] {100,200};
			for (i=0; i<X.Length; i++){
				System.Console.WriteLine("in:  X: {0,8:f4}  Y: {1,8:f4} Z: {2,4:f4}", X[i], Y[i], Z[i]);}
			System.Console.WriteLine("\nTransformation from utm32Eed50 to utm32Ewgs84:\n");
			error=Interface.Transform("utm32Eed50","utm32Ewgs84",X,Y,Z);
			if (error != Interface.KMSTR_Error.KMSTR_OK) 
				throw new Exception(Interface.GetKMSErrorMessage(error));
			for (i=0; i<X.Length; i++){
				System.Console.WriteLine("out: X: {0,8:f4}  Y: {1,8:f4} Z: {2,4:f4} ret: {3:d}", X[i], Y[i], Z[i],error);}
			System.Console.WriteLine("\nTransformation from utm32_wgs84 to utm32_ed50:\n");
			error=Interface.Transform("utm32_wgs84","utm32_ed50",X,Y,null);
			if (error != Interface.KMSTR_Error.KMSTR_OK) 
				throw new Exception(Interface.GetKMSErrorMessage(error));
			for (i=0; i<X.Length; i++){
				System.Console.WriteLine("out: X: {0,8:f4}  Y: {1,8:f4} Z: {2,4:f4} ret: {3:d}", X[i], Y[i], Z[i],error);}
		}
	}
}
