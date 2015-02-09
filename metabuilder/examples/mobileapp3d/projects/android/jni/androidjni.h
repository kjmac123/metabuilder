#ifndef _Included_com_metabuilder_mobileapp_AndroidJNI
#define _Included_com_metabuilder_mobileapp_AndroidJNI
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_metabuilder_mobileapp_AndroidJNI
 * Method:    OnAppInit
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_metabuilder_mobileapp_AndroidJNI_OnAppInit
  (JNIEnv *, jobject, jobject);

/*
 * Class:     com_metabuilder_mobileapp_AndroidJNI
 * Method:    OnAppShutdown
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_metabuilder_mobileapp_AndroidJNI_OnAppShutdown
  (JNIEnv *, jobject);

/*
 * Class:     com_metabuilder_mobileapp_AndroidJNI
 * Method:    OnAppPause
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_metabuilder_mobileapp_AndroidJNI_OnAppPause
  (JNIEnv *, jobject);

/*
 * Class:     com_metabuilder_mobileapp_AndroidJNI
 * Method:    OnAppResume
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_metabuilder_mobileapp_AndroidJNI_OnAppResume
  (JNIEnv *, jobject);

/*
 * Class:     com_metabuilder_mobileapp_AndroidJNI
 * Method:    OnViewInit
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_metabuilder_mobileapp_AndroidJNI_OnViewInit
  (JNIEnv *, jobject);

/*
 * Class:     com_metabuilder_mobileapp_AndroidJNI
 * Method:    OnSurfaceChanged
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_com_metabuilder_mobileapp_AndroidJNI_OnSurfaceChanged
  (JNIEnv *, jobject, jint, jint);

/*
 * Class:     com_metabuilder_mobileapp_AndroidJNI
 * Method:    OnDrawFrame
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_metabuilder_mobileapp_AndroidJNI_OnDrawFrame
  (JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif
