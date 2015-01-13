/**********************************
 Java Native Interface library
**********************************/
#include <jni.h>
#include <android/log.h>

#include "src/common.h"

/** This is the C++ implementation of the Java native method.
@param env Pointer to JVM environment
@param thiz Reference to Java this object
*/
extern "C"
JNIEXPORT void JNICALL
Java_hello_androidnative_HelloAndroidNative_helloandroidnativeNative( JNIEnv* env, jobject thiz )
{
	__android_log_print(ANDROID_LOG_INFO, "metabuilder", "[METABUILDER NATIVE] %s", "HELLO FROM NATIVE CODE!");
}
