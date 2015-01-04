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

bool CreateLink(const char* src, const char* dst)
{
	MB_LOGERROR("Links not supported");
	return false;
}

E_FileType GetFileType(const std::string& filepath)
{
	DWORD fileAttr = GetFileAttributesA(filepath.c_str());
	
	if (fileAttr == INVALID_FILE_ATTRIBUTES)
		return E_FileType_Missing;

	if (fileAttr & FILE_ATTRIBUTE_DIRECTORY)
		return E_FileType_Dir; 

	return E_FileType_File;
}

bool BuildFileListRecurse(std::vector<std::string>* fileList, const char* osInputDir, const char* includeFilePattern, const char* excludeDir)
{
	//Process dirs
	{
		WIN32_FIND_DATA fdFile;
		HANDLE hFind = NULL;

		char sPath[MB_MAX_PATH];
		mbHostPathJoin(sPath, osInputDir, "*.*");
    
		if((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE)
		{
			return false;
		}
    
		do
		{
			//Find first file will always return "."
			//    and ".." as the first two directories.
			if(strcmp(fdFile.cFileName, ".") != 0
			   && strcmp(fdFile.cFileName, "..") != 0)
			{
				//Build up our file path using the passed in
				//  [sDir] and the file/foldername we just found:
				mbHostPathJoin(sPath, osInputDir, fdFile.cFileName);
            
				//Is the entity a File or Folder?
				if(fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					if (!excludeDir || strcmp(fdFile.cFileName, excludeDir) != 0)
					{
						BuildFileListRecurse(fileList, sPath, includeFilePattern, excludeDir);
					}
				}
			}
		}
		while(FindNextFile(hFind, &fdFile)); //Find the next file.

		FindClose(hFind);
	}
    
	//Process files
	{
		WIN32_FIND_DATA fdFile;
		HANDLE hFind = NULL;
		char sPath[MB_MAX_PATH];
		mbHostPathJoin(sPath, osInputDir, includeFilePattern);
    
		if((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE)
		{
			return false;
		}
    
		do
		{
			//Find first file will always return "."
			//    and ".." as the first two directories.
			if(strcmp(fdFile.cFileName, ".") != 0
			   && strcmp(fdFile.cFileName, "..") != 0)
			{
				//Build up our file path using the passed in
				//  [sDir] and the file/foldername we just found:
				mbHostPathJoin(sPath, osInputDir, fdFile.cFileName);
            
				//Is the entity a File or Folder?
				if(fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					if (!excludeDir || strcmp(fdFile.cFileName, excludeDir) != 0)
					{
						BuildFileListRecurse(fileList, sPath, includeFilePattern, excludeDir);
					}
				}
				else
				{
					//Ignore hidden unix files.
					if (fdFile.cFileName[0] == '.')
						continue;
            
					char* fp = sPath;
					if (strstr(fp, "./") == fp)
					{
						fp += 2;
					}
					char buf[MB_MAX_PATH];
					mbNormaliseFilePath(buf, fp, '\\');
					fileList->push_back(buf);
				}
			}
		}
		while(FindNextFile(hFind, &fdFile)); //Find the next file.

	    FindClose(hFind);
	}
    return true;
}

void FileSetWorkingDir(const std::string& path)
{
    _chdir(path.c_str());
}

std::string FileGetWorkingDir()
{
    char workingDir[MB_MAX_PATH];
    return _getcwd(workingDir, sizeof(workingDir));
}

std::string	FileGetAbsPath(const std::string& path)
{
	char tmp[MB_MAX_PATH];
	return _fullpath(tmp, path.c_str(), sizeof(tmp));
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

