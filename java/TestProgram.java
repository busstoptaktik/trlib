public class TestProgram{
	public static void main(String args[]){
		String version=TrLib.GetVersion();
		System.out.println(version);
		TrLib.InitLibrary("..\\python\\Geoids\\");
		TrLib.CoordinateTransformation ct;
		try {
		ct=new TrLib.CoordinateTransformation("utm32_wgs84","utm32_ed50");}
		catch (Exception e){
			System.out.println("oh no!");
			return;}
		double X[]={5143200.1,450200.1};
		double Y[]={6143200.1,6200000.1};
		double Z[]={0,0};
		System.out.println("And here we go!");
		try {
		ct.Transform(X,Y,Z);}
		catch (Exception e){
			System.out.println("Oh no again!");}
		for (int i=0; i<X.length; i++){
			System.out.format("%.4f %.4f %.4f%n",X[i],Y[i],Z[i]);}
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
		try {
		ct.Transform(X,Y,Z);}
		catch (Exception e){
			System.out.println("Oh no again!");}
		long elapsed=System.currentTimeMillis()-start;
		System.out.format("Running time: %.4f s", ((double) elapsed)/1000.0);
		ct.Close();
		}
	}
	