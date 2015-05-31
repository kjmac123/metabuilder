#include "corelib/core.h"
#include "corelib/logicalfs.h"

#include "corelib/platform/platformfile.h"

ILogicalFile::~ILogicalFile()
{
}

ILogicalFile* LogicalFS_OpenBundleFile(const char* filepath)
{
    return Platform::File_OpenOSBundleFile(filepath);
}
