#include <stdio.h>
#include <stdlib.h>
#include "trlib_api.h"

int main(void) {
	int i,rc;
	double x[2]={512200.1,845200.1};
	double y[2]={6143200.0,6099200.2};
	double z[2]={20.0,30.0};
	double x_o[2],y_o[2],z_o[2];
	double x1,y1,z1;
	const double R2D=180.0/3.14159265;
	char ver[100];
	TR *trf;
	TR_GetVersion(ver,99);
	printf("TrLib version %s\n",ver);
	TR_InitLibrary("");
	TR_AllowUnsafeTransformations();
	trf=TR_Open("utm32_etrs89","geo_etrs89","");
	for(i=0;i<2;i++){
		printf("input (2d): %.4f %.4f\n",x[i],y[i]);}
	printf("First a test of the point transformation method:\n");
	rc=TR_TransformPoint(trf,x[0],y[0],0,&x1,&y1,&z1);
	printf("output: %.4f %.4f %.4f, ret: %d\n",x1*R2D,y1*R2D,z1,rc);
	printf("And the inverse:\n");
	rc=TR_InverseTransformPoint(trf,x1,y1,100.0,&x1,&y1,NULL);
	printf("output: %.4f %.4f %.4f, ret: %d\n",x1,y1,z1,rc);	
	printf("Then a test of the no-copy-in-place-method:\n");
	rc=TR_Transform2(trf,x,y,NULL,x_o,y_o,z_o,2);
	printf("Return code: %d\n",rc);
	for(i=0;i<2;i++){
		printf("output: %.4f %.4f %.4f\n",x_o[i]*R2D,y_o[i]*R2D,z_o[i]);}
	printf("And the inverse method:\n");
	rc=TR_InverseTransform2(trf,x_o,y_o,z_o,x_o,y_o,NULL,2);
	printf("Return code: %d\n",rc);
	for(i=0;i<2;i++){
		printf("output: %.4f %.4f %.4f\n",x_o[i],y_o[i],z_o[i]);}
	printf("Then a test of the copy-in-place-method:\n");
	for(i=0;i<2;i++){
		printf("input (2d): %.4f %.4f\n",x[i],y[i]);}
	rc=TR_Transform(trf,x,y,NULL,2);
	printf("Return code: %d\n",rc);
	for(i=0;i<2;i++){
		printf("output (2d): %.4f %.4f\n",x[i]*R2D,y[i]*R2D);}
	rc=TR_InverseTransform(trf,x,y,NULL,2);
	for(i=0;i<2;i++){
		printf("output-inverse (2d): %.4f %.4f\n",x[i],y[i]);}
	printf("Return code: %d\n",rc);
	TR_Close(trf);
	printf("And now testing crt-transformations: utm32Hetsr89_h_dvr90->crt_wgs84.\n");
	trf=TR_Open("utm32Hetrs89_h_dvr90","crt_wgs84","");
	z1=100.0;
	printf("Input: %.4f %.4f %.4f\n",x1,y1,z1);
	rc=TR_TransformPoint(trf,x1,y1,z1,&x_o[0],&y_o[0],&z_o[0]);
	printf("output: %.4f %.4f %.4f, ret: %d\n",x_o[0],y_o[0],z_o[0],rc);
	printf("And the inverse...\n");
	rc=TR_InverseTransformPoint(trf,x_o[0],y_o[0],z_o[0],&x1,&y1,&z1);
	printf("output: %.4f %.4f %.4f, ret: %d\n",x1,y1,z1,rc);
	
	printf("And now testing utm32Hetrs89_h_dvr90->geoEetrs89:\n");
	
	trf=TR_Open("utm32Hetrs89_h_dvr90","geoEetrs89","");
	rc=TR_Transform(trf,x,y,z,2);
	printf("Return code from 3D-transformation: %d\n",rc);
	for(i=0;i<2;i++){
		printf("output (3d): %.4f %.4f %.2f\n",x[i]*R2D,y[i]*R2D,z[i]);}
	TR_Close(trf);
	printf("Testing geoid info:\n");
	trf=TR_Open("utm32Eed50","utm32Ewgs84","egm96g.01");
	printf("Opened TR-object: %d\n",(trf!=NULL));
	TR_GeoidInfo(trf);
	TR_Close(trf);
	TR_TerminateLibrary();
	//int code=0,ok=0;
	//int code2=0;
/*TR_ExportToEPSG("utm32_euref89",&code,&code2);
	printf("Code: %d\n",code);
	char mlb[128];
	TR_ImportFromEPSG(code,0,mlb);
	printf("MLB: %s\n",mlb);
	ok=TR_ExportToEPSG("utm32Hetrf89_h_dvr90",&code,&code2);
	printf("Translating utm32Hetrf89_h_dvr90 to %d, %d\n",code,code2);
	mlb[0]='\0';
	ok=TR_ImportFromEPSG(code,code2,mlb);
	printf("And back again: %s, it was ok: %d\n",mlb,ok);
	ok=TR_ExportToEPSG("utm29_ed50",&code,&code2);
	printf("And we can use (intelligent) utm lookup: %d\n",code);*/
	exit(0);}

	