#include <jni.h>
#include <string.h>
#ifdef _MSC_VER
#include <stdint.h>
#else
#include <inttypes.h>
#endif
#include "trlib_api.h"
JNIEXPORT jstring JNICALL Java_TrLib_GetVersion(JNIEnv *env, jclass jc){
	char buf[128];
	TR_GetVersion(buf,128);
	return (*env)->NewStringUTF(env, buf);
}

JNIEXPORT jint JNICALL Java_TrLib_InitLibrary(JNIEnv *env, jclass jc, jstring folder){
	const char *str;
	int ok;
	str = (*env)->GetStringUTFChars(env, folder, NULL);
	if (str == NULL) 
		return 0;
	ok=TR_InitLibrary((char *) str);
	(*env)->ReleaseStringUTFChars(env, folder, str);
	return (jint) ok;
}

JNIEXPORT void JNICALL Java_TrLib_TerminateLibrary(JNIEnv *env, jclass jc){
	TR_TerminateLibrary();
}

JNIEXPORT void JNICALL Java_TrLib_TerminateThread(JNIEnv *env, jclass jc){
	TR_TerminateThread();
}

JNIEXPORT void JNICALL Java_TrLib_AllowUnsafeTransformations(JNIEnv *env, jclass jc){
	TR_AllowUnsafeTransformations();
}

JNIEXPORT void JNICALL Java_TrLib_ForbidUnsafeTransformations(JNIEnv *env, jclass jc){
	TR_ForbidUnsafeTransformations();
}

JNIEXPORT jint JNICALL Java_TrLib_GetLastError(JNIEnv *env, jclass jc){
	return (jint) TR_GetLastError();
}

JNIEXPORT jlong JNICALL Java_TrLib_Tropen(JNIEnv *env, jclass jc, jstring mlb1, jstring mlb2){
	const char *str1,*str2;
	TR *trf;
	str1 = (*env)->GetStringUTFChars(env, mlb1, NULL);
	if (str1 == NULL)
		return TR_ALLOCATION_ERROR;
	str2 = (*env)->GetStringUTFChars(env, mlb2, NULL);
	if (str2 == NULL){
		(*env)->ReleaseStringUTFChars(env, mlb1, str1);
		return TR_ALLOCATION_ERROR;}
	trf=TR_Open((char *) str1, (char *) str2, "");
	if (trf == NULL){
		(*env)->ReleaseStringUTFChars(env, mlb1, str1);
		(*env)->ReleaseStringUTFChars(env, mlb2, str2);
		return TR_ALLOCATION_ERROR;}
	return (jlong) (intptr_t) trf;
}

JNIEXPORT void JNICALL Java_TrLib_Trclose(JNIEnv *env, jclass jc, jlong addr){
	if (addr == 0)
		return;
	TR_Close( (TR*) (intptr_t) addr);
}

JNIEXPORT jint JNICALL Java_TrLib_Transform(JNIEnv *env, jclass jc, jlong addr, jdoubleArray X, jdoubleArray Y, jdoubleArray Z, jint n){
	jdouble *x,*y,*z;
	int rc;
	x=(*env)->GetDoubleArrayElements(env,X,NULL);
	if (x==NULL)
		return TR_ALLOCATION_ERROR;
	y=(*env)->GetDoubleArrayElements(env,Y,NULL);
	if (y==NULL){
		 (*env)->ReleaseDoubleArrayElements(env, X, x, 0);
		return TR_ALLOCATION_ERROR;
		}
	if (Z!=NULL){
		z=(*env)->GetDoubleArrayElements(env,Z,NULL);
		if (z==NULL){
			(*env)->ReleaseDoubleArrayElements(env, X, x, 0);
			(*env)->ReleaseDoubleArrayElements(env, Y, y, 0);
			return TR_ALLOCATION_ERROR;
		}}
	else
		z=NULL;
	rc= TR_Transform((TR*) (intptr_t) addr, (double *) x, (double *) y, (double*) z, (int) n);
	(*env)->SetDoubleArrayRegion(env,X,0,n,x);
	(*env)->SetDoubleArrayRegion(env,Y,0,n,y);
	if (Z!=NULL){
		(*env)->SetDoubleArrayRegion(env,Z,0,n,z);
		(*env)->ReleaseDoubleArrayElements(env,Z,z,0);}
	
	(*env)->ReleaseDoubleArrayElements(env,X,x,0);
	(*env)->ReleaseDoubleArrayElements(env,Y,y,0);
	return rc;
}
	
	
	




	