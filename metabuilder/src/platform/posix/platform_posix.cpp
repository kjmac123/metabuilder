#include "metabuilder_pch.h"

#include "../platform.h"
#include "freebsd/realpath.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <fnmatch.h>
#include <dirent.h>
#include <sys/stat.h> 
#include <fcntl.h>

#if defined(PLATFORM_OSX)
#include <mach/mach_time.h>
#endif

namespace Platform
{

#if defined(PLATFORM_OSX)
static F64				g_machTimeToNs;
static mach_timebase_info_data_t	g_timebase;
#endif

bool CreateDir(const char* osDir)
{
    struct stat statResult;
    int statresult = stat(osDir, &statResult);
    if (statresult == 0)
    {
        if (!S_ISDIR(statResult.st_mode))
        {
            MB_LOGERROR("File obstructing dir creation %s\n", osDir);
            return false;
        }
        return true;
    }
    
    if (mkdir(osDir, 0777) == -1)
    {
        MB_LOGERROR("Mkdir failed for %s\n", osDir);
        return false;
    }
    
//    MB_LOGDEBUG("Created dir %s\n", osDir);
    return true;
}

void Init()
{
#if defined(PLATFORM_OSX)
	mach_timebase_info(&g_timebase);
	g_machTimeToNs = ((F64)g_timebase.numer / (F64)g_timebase.denom);
#endif
}

void Shutdown()
{	
}

E_FileType GetFileType(const FilePath& filepath)
{
	struct stat statbuf;
    if (stat(filepath.c_str(), &statbuf) == -1)
	{
		return E_FileType_Missing;
	}
	else if(S_ISDIR(statbuf.st_mode))
	{
		return E_FileType_Dir;
	}
    else if(S_ISREG(statbuf.st_mode))
    {
        return E_FileType_File;
    }
	
    return E_FileType_Unknown;
}
    
void BuildFileListAddFile(
    const FilePath& parentDir,
    const FilePath& filename,
    const FilePath& fullPath,
    DirWalkFileInfoFunc fileInfoFunc,
    void* userdata)
{
    FileInfo fileInfo;
    fileInfo.parentDir = parentDir;
    fileInfo.filename = filename;
    fileInfo.fullPath = fullPath;
    fileInfo.fileType = GetFileType(fullPath);
    
    fileInfoFunc(fileInfo, userdata);
//    MB_LOGINFO("%s", fullPath.c_str());
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

void DirWalk(const FilePath& currentDirFullPath, DirWalkFileInfoFunc fileInfoFunc, void* userdata)
{
    //Recurse directories
    {
        const char* osDir = currentDirFullPath.GetLength() == 0 ? "." : currentDirFullPath.c_str();
        DIR* dir = opendir(osDir);
        if (!dir)
        {
            return;
        }

        struct dirent* dirEntry;
        while((dirEntry = readdir(dir)))
        {
            if (dirEntry->d_name[0] == '.')
                continue;
            
            FilePath childItem(dirEntry->d_name);
            
            FilePath childFullPath;
            childFullPath.Join(currentDirFullPath);
            childFullPath.Join(childItem);

            if (GetFileType(childFullPath) == E_FileType_Dir)
            {
                DirWalk(childFullPath, fileInfoFunc, userdata);
            }
            else
            {
                BuildFileListAddFile(currentDirFullPath, childItem, childFullPath, fileInfoFunc, userdata);
            }
        }
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

void FileSetWorkingDir(const std::string& path)
{
    if (chdir(path.c_str()) != 0)
    {
		MB_LOGERROR("Failed to change working dir to %s", path.c_str());
		mbExitError();    	
    }
}

std::string FileGetWorkingDir()
{
   	char workingDir[PATH_MAX];
	if (getcwd(workingDir, sizeof(workingDir)) == NULL)
 	{
		MB_LOGERROR("Failed to query working dir");
		mbExitError();
 	}

	return workingDir;
}

std::string FileGetAbsPath(const std::string& path)
{
   	char storage[PATH_MAX];
	if (/*FreeBSD::*/realpath(path.c_str(), storage) == NULL)
   	{
		MB_LOGERROR("Failed to get absolute path for %s", path.c_str());
		mbExitError();
   	}

	return storage;
}

char GetDirSep()
{
	return '/';
}
    
void LogError(const char* str)
{
	printf("%s", str);
}

void LogInfo(const char* str)
{
	printf("%s", str);
}

void LogDebug(const char* str)
{
	printf("%s", str);
}

F64 GetSystemTicksToSecondsScale()
{
#if defined(PLATFORM_OSX)
	return 1.0 / F64(MB_SECONDS_TO_NANOSECONDS) * g_machTimeToNs;
#else
	return 1.0 / F64(MB_SECONDS_TO_NANOSECONDS);
#endif
}

U64 GetSystemTicks()
{
#if defined(PLATFORM_OSX)
	return mach_absolute_time();
#else
	timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	U64 nanoseconds = (U64)t.tv_nsec;
	U64 seconds = (U64)t.tv_sec;
	nanoseconds += seconds * MB_SECONDS_TO_NANOSECONDS;
	return nanoseconds;
#endif
}

}

