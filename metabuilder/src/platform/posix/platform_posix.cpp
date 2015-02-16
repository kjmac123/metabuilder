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

bool CreateLink(const char* src, const char* dst)
{
	char existingSrcPath[PATH_MAX] = {0};
	ssize_t readlinkResult = readlink(dst, existingSrcPath, sizeof(existingSrcPath));
	if (readlinkResult > 0)
	{
		//link already exists
		if (!strcmp(existingSrcPath, src))
			return true;
			
		//Link is to a different location.
		MB_LOGERROR("Cannot create symbolic link from %s to %s as destination is already linked to %s", src, dst, existingSrcPath);
		mbExitError();
		return 0;
	}

	int result = symlink(src, dst);
	if (result == 0)
		return true;

	MB_LOGERROR("Failed to create symbolic link from %s to %s", src, dst);
	return false;
}

E_FileType GetFileType(const std::string& filepath)
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

void BuildFileListRecurse(std::vector<std::string>* fileList, const char* parentFilepath)
{
    if (GetFileType(parentFilepath) == E_FileType_File)
    {
        fileList->push_back(parentFilepath);
        return;
    }

	if (parentFilepath[0] == '\0')
	{
		parentFilepath = ".";
	}
	
	DIR* dir = opendir(parentFilepath);
	if (!dir)
    {
        return;
    }
    
    struct dirent* dirEntry;
    
    std::vector<std::string> dirStack;
    while((dirEntry = readdir(dir)))
    {
		if (!strcmp(dirEntry->d_name, ".") || !strcmp(dirEntry->d_name, ".."))
			continue;
        
        char childFilepath[FILENAME_MAX];
        sprintf(childFilepath, "%s/%s", parentFilepath, dirEntry->d_name);
        
        if (GetFileType(childFilepath) == E_FileType_Dir)
        {
            dirStack.push_back(childFilepath);
        }
        else
        {
            //Ignore hidden unix files.
            if (dirEntry->d_name[0] == '.')
                continue;
            
            char* fp = childFilepath;
            if (strstr(fp, "./") == fp)
            {
                fp += 2;
            }
            fileList->push_back(fp);
        }
    }
    
    closedir(dir);

    for (int i = 0; i < (int)dirStack.size(); ++i)
    {
        BuildFileListRecurse(fileList, dirStack[i].c_str());
    }
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

