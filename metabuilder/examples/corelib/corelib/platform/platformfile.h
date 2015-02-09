#ifndef CORELIB_PLATFORMFILE_H
#define CORELIB_PLATFORMFILE_H

#include "corelib/logicalfs.h"

namespace Platform
{
    class FileImpl;
    
    class File : public ILogicalFile
    {
    public:
                        ~File();

        S64             Read(void* dst, S64 toRead);
        S64             Write(S64 toWrite, void* data);

        S64             GetLength() const;
        
    private:
                        File();
        void            Close();
        
        friend ILogicalFile*    File_OpenOSFile(const char* filepath, E_FileMode mode);
        
        FileImpl*       m_impl;
    };
    
	void		File_Init();
	void		File_Shutdown();

    ILogicalFile*       File_OpenOSBundleFile(const char* filepath);
    ILogicalFile*       File_OpenOSFile(const char* filepath, E_FileMode mode);
}

#endif
