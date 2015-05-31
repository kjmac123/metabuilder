#ifndef LOGICALFS_H
#define LOGICALFS_H

enum E_FileMode
{
    E_FileMode_ReadBinary
};

class ILogicalFile
{
public:
    virtual         ~ILogicalFile();
    
    virtual S64     Read(void* dst, S64 toRead) = 0;
    virtual S64     Write(S64 toWrite, void* data) = 0;
    
    virtual S64     GetLength() const = 0;
};

ILogicalFile* LogicalFS_OpenBundleFile(const char* filepath);

#endif
