/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class TrLib */

#ifndef _Included_TrLib
#define _Included_TrLib
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     TrLib
 * Method:    GetVersion
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_TrLib_GetVersion
  (JNIEnv *, jclass);

/*
 * Class:     TrLib
 * Method:    InitLibrary
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_TrLib_InitLibrary
  (JNIEnv *, jclass, jstring);

/*
 * Class:     TrLib
 * Method:    TerminateLibrary
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_TrLib_TerminateLibrary
  (JNIEnv *, jclass);

/*
 * Class:     TrLib
 * Method:    tropen
 * Signature: (Ljava/lang/String;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_TrLib_tropen
  (JNIEnv *, jclass, jstring, jstring);

/*
 * Class:     TrLib
 * Method:    trclose
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_TrLib_trclose
  (JNIEnv *, jclass, jlong);

/*
 * Class:     TrLib
 * Method:    tr
 * Signature: (J[D[D[DI)I
 */
JNIEXPORT jint JNICALL Java_TrLib_tr
  (JNIEnv *, jclass, jlong, jdoubleArray, jdoubleArray, jdoubleArray, jint);

#ifdef __cplusplus
}
#endif
#endif
