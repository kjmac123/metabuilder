#include "metabuilder_pch.h"

#include <Windows.h>
#include <direct.h>

#include "platform/platform.h"

namespace Platform
{

void Init()
{
}

void Shutdown()
{
}

bool CreateDir(const char* osDir)
{
    DWORD ftyp = GetFileAttributesA(osDir);
    if (ftyp != INVALID_FILE_ATTRIBUTES)
    {
        if ((ftyp & FILE_ATTRIBUTE_DIRECTORY) != 0)
            return true;

        MB_LOGERROR("File obstructing dir creation %s", osDir);
        return false;
    }

    if (!CreateDirectoryA(osDir, NULL) && GetLastError() != ERROR_PATH_NOT_FOUND)
    {
        MB_LOGERROR("Mkdir failed for %s", osDir);
        return false;
    }

    return true;
}

static E_FileType GetFileType(DWORD fileAttr)
{
    if (fileAttr == INVALID_FILE_ATTRIBUTES)
        return E_FileType_Missing;

    if (fileAttr & FILE_ATTRIBUTE_DIRECTORY)
        return E_FileType_Dir;

    return E_FileType_File;
}

E_FileType GetFileType(const FilePath& filepath)
{
    DWORD fileAttr = GetFileAttributesA(filepath.c_str());
    return GetFileType(fileAttr);
}

void BuildFileListAddFile(
    const FilePath& parentDir,
    const FilePath& filename, 
    const FilePath& fullPath,
    DirWalkFileInfoFunc fileInfoFunc,
    void* userdata)
{
    FileInfo fileInfo;
    fileInfo.attributes.hidden = false;
    fileInfo.parentDir = parentDir;
    fileInfo.filename = filename;
    fileInfo.fullPath = fullPath;
    fileInfo.fileType = GetFileType(fullPath);

    fileInfoFunc(fileInfo, userdata);
}
    
void BuildFileListAddFile(
    const FilePath& fullPath,
    DirWalkFileInfoFunc fileInfoFunc,
    void* userdata)
{
    FilePath split1, split2, dir;
    if (fullPath.SplitLast(&split1, &split2))
    {
        BuildFileListAddFile(split1, split2, fullPath, fileInfoFunc, userdata);
    }
    else
    {
        BuildFileListAddFile(FilePath(), fullPath, fullPath, fileInfoFunc, userdata);
    }
}

static bool IgnoreSpecial(const char* filename, DWORD fileAttr)
{
    if (strcmp(filename, ".") == 0 ||
        strcmp(filename, "..") == 0 ||
        (fileAttr != INVALID_FILE_ATTRIBUTES) && (fileAttr & FILE_ATTRIBUTE_HIDDEN))
    {
        return true;
    }

    return false;
}

void DirWalk(const FilePath& dir, DirWalkFileInfoFunc fileInfoFunc, void* userdata)
{
    //Recurse directories
    {
        WIN32_FIND_DATA fdFile;
        HANDLE hFind;
        FilePath thisSearchPattern;
        thisSearchPattern.Join(dir);
        thisSearchPattern.Join(FilePath("*"));
        if ((hFind = FindFirstFile(thisSearchPattern.c_str(), &fdFile)) == INVALID_HANDLE_VALUE)
        {
            return;
        }

        do
        {
            if (GetFileType(fdFile.dwFileAttributes) == E_FileType_Dir)
            {
                if (!IgnoreSpecial(fdFile.cFileName, fdFile.dwFileAttributes))
                {
                    FilePath childDir = dir;
                    childDir.Join(FilePath(fdFile.cFileName));
                    
                    DirWalk(childDir, fileInfoFunc, userdata);
                }
            }
        } 
        while (FindNextFile(hFind, &fdFile)); //Find the next file.

        FindClose(hFind);
    }

    {
        WIN32_FIND_DATA fdFile;
        HANDLE hFind;
        FilePath thisSearchPattern;
        thisSearchPattern.Join(dir);
        thisSearchPattern.Join(FilePath("*"));
        if ((hFind = FindFirstFile(thisSearchPattern.c_str(), &fdFile)) == INVALID_HANDLE_VALUE)
        {
            return;
        }

        do
        {
            if (!IgnoreSpecial(fdFile.cFileName, fdFile.dwFileAttributes))
            {
                if (GetFileType(fdFile.dwFileAttributes) == E_FileType_File)
                {
                    FilePath childFilename(fdFile.cFileName);

                    FilePath childFullPath;
                    childFullPath.Join(dir);
                    childFullPath.Join(childFilename);
                    BuildFileListAddFile(dir, childFilename, childFullPath, fileInfoFunc, userdata);
                }
            }
        } 
        while (FindNextFile(hFind, &fdFile)); //Find the next file.

        FindClose(hFind);
    }
}
    
void BuildFileListDir(const FilePath& dir, DirWalkFileInfoFunc fileInfoFunc, void* userdata)
{
    Platform::DirWalk(dir, fileInfoFunc, userdata);
}

void BuildFileListFile(const FilePath& filePath, DirWalkFileInfoFunc fileInfoFunc, void* userdata)
{
    BuildFileListAddFile(filePath, fileInfoFunc, userdata);
}

void FileSetWorkingDir(const FilePath& path)
{
    _chdir(path.c_str());
}

FilePath FileGetWorkingDir()
{
    char workingDir[MB_MAX_PATH];
    return FilePath(_getcwd(workingDir, sizeof(workingDir)));
}

FilePath FileGetAbsPath(const FilePath& path)
{
    char tmp[MB_MAX_PATH];
    return FilePath(_fullpath(tmp, path.c_str(), sizeof(tmp)));
}

char GetDirSep()
{
    return '\\';
}

void LogError(const char* str)
{
    OutputDebugString(str);
    printf("%s", str);
}

void LogInfo(const char* str)
{
    OutputDebugString(str);
    printf("%s", str);
}

void LogDebug(const char* str)
{
    OutputDebugString(str);
    printf("%s", str);
}

F64 GetSystemTicksToSecondsScale()
{
    LARGE_INTEGER f;
    QueryPerformanceFrequency(&f);
    return 1.0 / (F64)f.QuadPart;
}

U64 GetSystemTicks()
{
    LARGE_INTEGER ticks;
    QueryPerformanceCounter(&ticks);
    return ticks.QuadPart;
}

}

