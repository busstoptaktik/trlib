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
		}
	}
	