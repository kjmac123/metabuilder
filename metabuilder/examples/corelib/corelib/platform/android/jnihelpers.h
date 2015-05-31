#ifndef JNIHELPERS_H
#define JNIHELPERS_H

#include <jni.h>

namespace Platform
{
    
struct JNIInitInfo
{
    JNIEnv* env;
    jobject jvm;
    jobject assetManager;
};
    
}

#endif
