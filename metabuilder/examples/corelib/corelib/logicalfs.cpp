#include "corelib/core.h"
#include "corelib/logicalfs.h"

Platform::File* LogicalFS_OpenBundleFile(const char* filepath, Platform::E_FileMode mode)
{
    char osFilePath[PLATFORM_MAX_PATH];
    core_sprintf(osFilePath, sizeof(osFilePath), "%s/%s", Platform::File_GetBundleRoot(), filepath);
    
    return File_OpenOSFile(osFilePath, mode);
}
