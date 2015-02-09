#include "corelib/core.h"
#include "corelib/logicalfs.h"
#include "corelib/platform/platformfile.h"

#import <Foundation/Foundation.h>

class ILogicalFile;

namespace Platform
{
    static char g_bundleRoot[PLATFORM_MAX_PATH];

    ILogicalFile* File_OpenOSBundleFile(const char* filepath)
    {
        char osFilePath[PLATFORM_MAX_PATH];
        core_sprintf(osFilePath, sizeof(osFilePath), "%s/%s", g_bundleRoot, filepath);
        
        return File_OpenOSFile(osFilePath, E_FileMode_ReadBinary);
    }
    
    void File_Init()
    {
        core_strcpy(g_bundleRoot, sizeof(g_bundleRoot), [[[NSBundle mainBundle] bundlePath] UTF8String]);
    }
    
    void File_Shutdown()
    {
        
    }
    
    
}
