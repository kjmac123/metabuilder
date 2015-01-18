#include <jni.h>

#include "src/core.h"
#include "src/common.h"

extern "C"
JNIEXPORT void JNICALL
Java_com_metabuilder_mobileapp_MobileApp_MobileAppNative_Init(JNIEnv* env, jobject obj)
{
	Platform::Init();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_metabuilder_mobileapp_MobileApp_MobileAppNative_Shutdown(JNIEnv* env, jobject obj)
{
	Platform::Shutdown();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_metabuilder_mobileapp_MobileApp_MobileAppNative_Run(JNIEnv* env, jobject obj)
{
	CommonCodeTest();
}
