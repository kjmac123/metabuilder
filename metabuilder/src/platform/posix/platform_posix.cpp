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

namespace Platform
{

#if defined(PLATFORM_IOS) || defined(PLATFORM_OSX)
static F64							g_machTimeToNs;
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
#if defined(PLATFORM_IOS) || defined(PLATFORM_OSX)
	mach_timebase_info(&PlatformThread_Timebase);
	g_MachTimeToNs = ((F64)PlatformThread_Timebase.numer / (F64)PlatformThread_Timebase.denom);
#endif
}

void Shutdown()
{	
}

bool CreateLink(const char* src, const char* dst)
{
	char existingSrcPath[MB_MAX_PATH] = {0};
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

void NormaliseFilePath(char* outFilePath, const char* inFilePath)
{
    bool preceedingSlash = false;
    
    outFilePath[0] = 0;
    char* outCursor = outFilePath;
    for (const char* inCursor = inFilePath; *inCursor; ++inCursor)
    {
		char c = *inCursor;
        //Normalise slashes
        if (c == '\\')
            c = '/';
        
        //Ignore duplicate slashes
        if (c == '/')
        {
            if (preceedingSlash)
                continue;
            preceedingSlash = true;
        }
        else
        {
            preceedingSlash = false;
        }
        
        *outCursor = c;
        ++outCursor;
    }
	*outCursor = '\0';
}

E_FileType GetFileType(const std::string& filepath)
{
	struct stat statbuf;
    if (stat(filepath.c_str(), &statbuf) == -1)
	{
		return E_FileType_Missing;
	}
	
	if(S_ISDIR(statbuf.st_mode))
	{
		return E_FileType_Dir;
	}
	
	return E_FileType_File;
}

bool BuildFileListRecurse(std::vector<std::string>* fileList, const char* osInputDir, const char* includeFilePattern, const char* excludeDirs)
{
	if (osInputDir[0] == '\0')
	{
		osInputDir = ".";
	}
	
	DIR* dir = opendir(osInputDir);
	if (!dir)
    {
//        Debug::Error("Failed to open directory %s", osInputDir);
        return false;
    }
    
    struct dirent* dirEntry;
    
    std::vector<std::string> dirStack;
    while((dirEntry = readdir(dir)))
    {
		if (!strcmp(dirEntry->d_name, ".") || !strcmp(dirEntry->d_name, ".."))
			continue;
        
        char filePath[FILENAME_MAX];
		
        sprintf(filePath, "%s/%s", osInputDir, dirEntry->d_name);
        
        struct stat statResult;
		if (stat(filePath, &statResult) == -1)
        {
//            Debug::Error("Stat failed for %s", filePath);
            return false;
		}
        
        if (S_ISDIR(statResult.st_mode))
        {
			//If not excluded...
			bool filterMatched = false;
			
			if (excludeDirs)
			{
				char tmp[MB_MAX_PATH];
				strcpy(tmp, excludeDirs);
				char* current = strtok (tmp,",");
				while (current != NULL)
				{
					if (strcmp(current, dirEntry->d_name) == 0)
					{
						filterMatched = true;
						break;
					}
					
					current = strtok(NULL, ",");
				}
			}
			
			if (!filterMatched)
			{
				dirStack.push_back(filePath);
			}
        }
        else
        {
            //Ignore hidden unix files.
            if (dirEntry->d_name[0] == '.')
                continue;
            
            if (!includeFilePattern || fnmatch(includeFilePattern, dirEntry->d_name, 0) == 0)
            {
				char* fp = filePath;
				if (strstr(fp, "./") == fp)
				{
					fp += 2;
				}
				fileList->push_back(fp);
            }
        }
    }
    
    closedir(dir);

    for (int i = 0; i < (int)dirStack.size(); ++i)
    {
        BuildFileListRecurse(fileList, dirStack[i].c_str(), includeFilePattern, excludeDirs);
    }
    
    return true;
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
   	char workingDir[MB_MAX_PATH];
	if (getcwd(workingDir, sizeof(workingDir)) == NULL)
 	{
		MB_LOGERROR("Failed to query working dir");
		mbExitError();
 	}

	return workingDir;
}

std::string FileGetAbsPath(const std::string& path)
{
   	char storage[MB_MAX_PATH];
	if (/*FreeBSD::*/realpath(path.c_str(), storage) == NULL)
   	{
		MB_LOGERROR("Failed to get absolute path for %s", path.c_str());
		mbExitError();
   	}

	return storage;
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
#if defined(PLATFORM_IOS) || defined(PLATFORM_OSX)
	return 1.0 / F64(MB_SECONDS_TO_NANOSECONDS) * g_machTimeToNs;
#else
	return 1.0 / F64(MB_SECONDS_TO_NANOSECONDS);
#endif
}

U64 GetSystemTicks()
{
#if defined(PLATFORM_IOS) || defined(PLATFORM_OSX)
	U64 machTime = mach_absolute_time();
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

