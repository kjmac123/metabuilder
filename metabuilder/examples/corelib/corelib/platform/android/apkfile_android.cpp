#include "corelib/core.h"
#include "corelib/platform/platform.h"

#include <android/log.h>
#include <android/asset_manager.h>

#include "corelib/logicalfs.h"

#include <jni.h>

namespace Platform
{
    extern AAssetManager* g_assetManager;
    
    class ApkFile : public ILogicalFile
    {
    public:
        virtual             ~ApkFile();
        
        virtual S64         Read(void* dst, S64 toRead);
        virtual S64         Write(S64 toWrite, void* data);
        
        virtual S64         GetLength() const;
        
    private:
                            ApkFile();
        void                Close();
        
        friend      ILogicalFile* ApkFile_OpenFile(const char* filepath);
        
        AAsset*     m_asset;
        S64         m_length;
    };

    ApkFile::ApkFile()
    {
        m_asset = 0;
        m_length = 0;
    }
    
    ApkFile::~ApkFile()
    {
        Close();
    }
    
    S64 ApkFile::Read(void* dst, S64 toRead)
    {
        return AAsset_read(m_asset, dst, toRead);
    }
    
    S64 ApkFile::Write(S64 toWrite, void* data)
    {
        MB_ASSERT(0);
        return 0;
    }
    
    S64 ApkFile::GetLength() const
    {
        return AAsset_getLength(m_asset);
    }
    
    void ApkFile::Close()
    {
        AAsset_close(m_asset);
    }
                                              
    ILogicalFile* ApkFile_OpenFile(const char* filepath)
    {
        ApkFile* f = NULL;

        MB_LOGINFO("ApkFile_OpenFile %s", filepath);
        AAsset* asset = AAssetManager_open(g_assetManager, filepath, AASSET_MODE_UNKNOWN);
        
        if (asset)
        {
            f = new ApkFile();
            f->m_asset = asset;
        }
        
        MB_LOGINFO("ApkFile_OpenFile %s returning 0x%p", filepath, f);
        return f;
    }
}
