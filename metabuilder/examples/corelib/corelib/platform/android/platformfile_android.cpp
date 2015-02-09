#include "corelib/core.h"
#include "corelib/logicalfs.h"

namespace Platform
{
    ILogicalFile* ApkFile_OpenFile(const char* filepath);
    
    ILogicalFile* File_OpenOSBundleFile(const char* filepath)
    {
        return ApkFile_OpenFile(filepath);
    }
}
