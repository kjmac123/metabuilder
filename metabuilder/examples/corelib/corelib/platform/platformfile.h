#ifndef CORELIB_PLATFORMFILE_H
#define CORELIB_PLATFORMFILE_H

namespace Platform
{
    enum E_FileMode
    {
        E_FileMode_ReadBinary
    };
    
    class FileImpl;
    
    class File
    {
    public:
                        ~File();

        S64             Read(void* dst, S64 toRead);
        S64             Write(S64 toWrite, void* data);

        S64             GetLength() const;
        
    private:
                        File();
        void            Close();
        
        friend File*    File_OpenOSFile(const char* filepath, E_FileMode mode);
        
        FileImpl*       m_impl;
    };
    
	void		File_Init();
	void		File_Shutdown();
    
    const char* File_GetBundleRoot();

    File*       File_OpenOSFile(const char* filepath, E_FileMode mode);
}

#endif
