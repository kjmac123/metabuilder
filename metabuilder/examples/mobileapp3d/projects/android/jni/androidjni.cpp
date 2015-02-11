#include "corelib/core.h"
#include "corelib/appdelegate.h"

#include "app/mainview.h"

CoreAppDelegate*    g_coreAppDelegate;
MainView*           g_mainView;

#include "corelib/platform/android/jnihelpers.h"

extern "C"
{

    JNIEXPORT void JNICALL Java_com_metabuilder_mobileapp_AndroidJNI_OnAppInit(JNIEnv *env, jobject jvm, jobject assetManager)
    {
        g_coreAppDelegate = new CoreAppDelegate;
        Platform::JNIInitInfo info;
        info.env = env;
        info.jvm = jvm;
        info.assetManager = assetManager;
        g_coreAppDelegate->OnInit(&info);
    }

    JNIEXPORT void JNICALL Java_com_metabuilder_mobileapp_AndroidJNI_OnAppShutdown(JNIEnv *, jobject)
    {
        g_coreAppDelegate->OnShutdown();
    }

    JNIEXPORT void JNICALL Java_com_metabuilder_mobileapp_AndroidJNI_OnAppPause(JNIEnv *, jobject)
    {
        g_coreAppDelegate->OnPause();
    }

    JNIEXPORT void JNICALL Java_com_metabuilder_mobileapp_AndroidJNI_OnAppResume(JNIEnv *, jobject)
    {
        g_coreAppDelegate->OnResume();
    }

    JNIEXPORT void JNICALL Java_com_metabuilder_mobileapp_AndroidJNI_OnViewInit(JNIEnv *, jobject)
    {
        g_mainView = new MainView();
        g_mainView->OnInit();
    }

    JNIEXPORT void JNICALL Java_com_metabuilder_mobileapp_AndroidJNI_OnViewShutdown(JNIEnv *, jobject)
    {
        g_mainView->OnShutdown();
        delete g_mainView;
        g_mainView = NULL;
    }
    
    JNIEXPORT void JNICALL Java_com_metabuilder_mobileapp_AndroidJNI_OnSurfaceChanged(JNIEnv *, jobject, jint w, jint h)
    {
        if (g_mainView)
        {
            ViewSettings viewSettings;
            viewSettings.width = w;
            viewSettings.height = h;
            g_mainView->OnSurfaceChanged(viewSettings);
        }
    }

    JNIEXPORT void JNICALL Java_com_metabuilder_mobileapp_AndroidJNI_OnDrawFrame(JNIEnv *, jobject)
    {
        if (g_mainView)
        {
            g_mainView->OnDrawFrame();
        }
    }

}
