#include <jni.h>

#include "src/core.h"
#include "src/common.h"

extern "C"
JNIEXPORT void JNICALL
Java_com_metabuilder_mobileapp_mobileapp_mobileappNativeInit(JNIEnv* env, jobject obj)
{
	Core_Init();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_metabuilder_mobileapp_mobileapp_mobileappNativeShutdown(JNIEnv* env, jobject obj)
{
	Core_Shutdown();
}

extern "C"
JNIEXPORT double JNICALL
Java_com_metabuilder_mobileapp_mobileapp_mobileappNativeRun(JNIEnv* env, jobject obj)
{
    return Core_GetElapsedTimeSeconds();
}
