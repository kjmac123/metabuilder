#include "corelib/core.h"
#include "corelib/platform/platform.h"
#include "corelib/platform/platformfile.h"

namespace Platform
{
    class FileImpl
    {
    public:
        FILE*   handle;
        S64     length;
        bool    lengthSet;
    };
    
    
    File::File()
    {
        m_impl = new FileImpl;
        memset(m_impl, 0, sizeof(FileImpl));
    }
    
    File::~File()
    {
        Close();
        delete m_impl;
    }
    
    S64 File::Read(void* dst, S64 toRead)
    {
        return static_cast<S64>(fread(dst, 1, static_cast<size_t>(toRead), m_impl->handle));
    }
    
    S64 File::Write(S64 toWrite, void* data)
    {
        return static_cast<S64>(fwrite(data, 1, static_cast<size_t>(toWrite), m_impl->handle));
    }
    
    S64 File::GetLength() const
    {
        if (!m_impl->lengthSet)
        {
            S64 oldPos = ftell(m_impl->handle);
            int result;
            result = fseek(m_impl->handle, 0, SEEK_END);
            if (result == -1)
                return -1;
            m_impl->length = ftell(m_impl->handle);
            result = fseek(m_impl->handle, static_cast<size_t>(oldPos), SEEK_SET);
            if (result == -1)
                return -1;
            
            m_impl->lengthSet = true;
        }
        
        return m_impl->length;
    }

    void File::Close()
    {
        MB_ASSERT(m_impl->handle);
        fclose(m_impl->handle);
    }

    ILogicalFile* File_OpenOSFile(const char* filepath, E_FileMode mode)
    {
        MB_ASSERT(mode == E_FileMode_ReadBinary);
        
        FILE* handle = fopen(filepath, "rb");
        if (handle)
        {
            File* f = new File();
            f->m_impl->handle = handle;
            return f;
        }
        
        return NULL;
    }
}
