using System;
using System.Collections.Generic;
using System.Text;
using System.Diagnostics;

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
			System.Console.WriteLine("\nTransformation from utm32Ewgs84 to utm33Eed50, using CoordinateTransformation class:\n"); 
			CoordinateTransformation tr=new CoordinateTransformation("utm32Ewgs84","utm33Eed50");
			System.Console.WriteLine("First a transformation of a single point:"); 	
			Point pt =new Point(512200.1,6143200.2,100.0);
			System.Console.WriteLine("In: X:{0:f6}  Y:{1:f6}  Z:{2:f6}",pt.x,pt.y,pt.z);
			pt=tr.Transform(pt);
			System.Console.WriteLine("out: X:{0:f6}  Y:{1:f6}  Z:{2:f6} ret:{3:d}",pt.x,pt.y,pt.z,pt.return_code);
			double[] X =new double[] {512200.1,512499.1};
			double[] Y =new double[] {6143200.1,6143300.1};
			double[] Z =new double[] {100,200};
			System.Console.WriteLine("Then an array:"); 
		        for (i=0; i<X.Length; i++){
				System.Console.WriteLine("in: X: {0,8:f4}  Y: {1,8:f4} Z: {2,4:f4}", X[i], Y[i], Z[i]);}
			error=tr.TransformArray(X,Y,Z);
			for (i=0; i<X.Length; i++){
				System.Console.WriteLine("out: X: {0,8:f4}  Y: {1,8:f4} Z: {2,4:f4} ret: {3:d}", X[i], Y[i], Z[i],error);}
			/* and now for a speed test */
			int n=50000;
			string elapsedTime;
			Stopwatch stopWatch = new Stopwatch();
			TimeSpan ts;
			System.Console.WriteLine("Speed test with {0:d} points:",n);
			X=new double[n];
			Y=new double[n];
			Z=new double[n];
			for (i=0; i<n ; i++){
				X[i]=512200.0+((double) i)/n*1000.0;
				Y[i]=6143200.0+((double) i)/n*1000.0;
				Z[i]=100.0+((double) i)/n*10.0;}
			stopWatch.Start();
			error=tr.TransformArray(X,Y,Z);
			stopWatch.Stop();
			ts = stopWatch.Elapsed;
				// Format and display the TimeSpan value.
			elapsedTime = String.Format("{0:00}:{1:00} seconds",ts.Seconds,ts.Milliseconds / 10);
			Console.WriteLine("RunTime, class-method: " + elapsedTime);	
			tr.Close(); /*remember to close the transformation in order to free memory in shared library */
				
       
				
				
		}
	}
}
