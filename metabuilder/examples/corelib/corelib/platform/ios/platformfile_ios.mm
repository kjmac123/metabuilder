#include "corelib/core.h"
#include "corelib/platform/platform.h"

#import <Foundation/Foundation.h>

namespace Platform
{
    static char g_bundleRoot[PLATFORM_MAX_PATH];
    
    const char* File_GetBundleRoot()
    {
        return g_bundleRoot;
    }


    
    void File_Init()
    {
        core_strcpy(g_bundleRoot, sizeof(g_bundleRoot), [[[NSBundle mainBundle] bundlePath] UTF8String]);
    }
    
    void File_Shutdown()
    {
        
    }
    
    
}
