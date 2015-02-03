#ifndef LOGICALFS_H
#define LOGICALFS_H

#include "corelib/platform/platformfile.h"

Platform::File* LogicalFS_OpenBundleFile(const char* filepath, Platform::E_FileMode mode);

#endif
