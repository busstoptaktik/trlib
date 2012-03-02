public class TestProgram{
	public static void main(String args[]){
		String version=TrLib.GetVersion();
		System.out.println("TrLib version is: "+version);
		TrLib.InitLibrary("");
		TrLib.SetThreadMode(false);
		TrLib.TR_Error err;
		TrLib.CoordinateTransformation ct;
		ct=new TrLib.CoordinateTransformation("utm32Ewgs84","utm32Eed50");
		double X[]={5143200.1,450200.1};
		double Y[]={6143200.1,6200000.1};
		double Z[]={100,200};
		System.out.println("And here we go!");
		err=ct.Transform(X,Y,Z);
		for (int i=0; i<X.length; i++){
			System.out.format("%.4f %.4f %.4f%n",X[i],Y[i],Z[i]);}
		System.out.println("And we can call overloaded method with just two args (X,Y):");
		X=new double[]{5143200.1,450200.1};
		Y=new double[]{6143200.1,6200000.1};
		err=ct.Transform(X,Y);
		for (int i=0; i<X.length; i++){
			System.out.format("%.4f %.4f %n",X[i],Y[i]);}
		ct.Close();
		int n=50000;	
		System.out.format("Speed test (utm32Ewgs84 to utm33Eed50) with %d points:%n",n);
		X=new double[n];
		Y=new double[n];
		Z=new double[n];
		int i;
		for (i=0; i<n ; i++){
			X[i]=512200.0+((double) i)/n*1000.0;
			Y[i]=6143200.0+((double) i)/n*1000.0;
			Z[i]=100.0+((double) i)/n*10.0;}
		try{
		ct=new TrLib.CoordinateTransformation("utm32Ewgs84","utm33Eed50");}
		catch (Exception e){
			System.out.println("oh no!");
			return;}
		long start=System.currentTimeMillis();
		ct.Transform(X,Y,Z);
		long elapsed=System.currentTimeMillis()-start;
		System.out.format("Running time: %.4f s", ((double) elapsed)/1000.0);
		ct.Close();
		}
	}
	