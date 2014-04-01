#include "metabuilder_pch.h"

#ifdef PLATFORM_WINDOWS

#include <Windows.h>
#include <direct.h>


#include "common.h"

#include "platform/platform.h"

bool _mbaCreateDir(const char* osDir)
{
    DWORD ftyp = GetFileAttributesA(osDir);
    if (ftyp != INVALID_FILE_ATTRIBUTES)
    {
        if ((ftyp & FILE_ATTRIBUTE_DIRECTORY) != 0)
            return true;

//        Debug::Error("File obstructing dir creation %s", osDir);
        return false;
    }
    
    if (!CreateDirectoryA(osDir, NULL) && GetLastError() != ERROR_PATH_NOT_FOUND)
    {
//        Debug::Error("Mkdir failed for %s", osDir);
        return false;
    }
    
//    Debug::Info("Created dir %s", osDir);
    return true;
}

bool mbaCreateLink(const char*, const char*)
{
	mbExitError();
	return false;
}

void mbaNormaliseFilePath(char* outFilePath, const char* inFilePath)
{
    outFilePath[0] = 0;
    char* outCursor = outFilePath;
    for (const char* inCursor = inFilePath; *inCursor; ++inCursor)
    {
		char c = *inCursor;
        //Normalise slashes
        if (c == '/')
            c = '\\';
        
        *outCursor = c;
        ++outCursor;
    }
	*outCursor = '\0';
}

E_FileType mbaGetFileType(const std::string& filepath)
{
	DWORD fileAttr = GetFileAttributesA(filepath.c_str());
	
	if (fileAttr == INVALID_FILE_ATTRIBUTES)
		return E_FileType_Missing;

	if (fileAttr & FILE_ATTRIBUTE_DIRECTORY)
		return E_FileType_Dir; 

	return E_FileType_File;
}

bool mbaBuildFileListRecurse(std::vector<std::string>* fileList, const char* osInputDir, const char* includeFilePattern, const char* excludeDir)
{
    WIN32_FIND_DATA fdFile;
    HANDLE hFind = NULL;
    
    char sPath[2048];
    
    //Specify a file mask. *.* = We want everything!
    sprintf(sPath, "%s\\%s", osInputDir, includeFilePattern);
    
    if((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE)
    {
        printf("Path not found: [%s]\n", osInputDir);
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
            sprintf(sPath, "%s\\%s", osInputDir, fdFile.cFileName);
            
            //Is the entity a File or Folder?
            if(fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
				if (strcmp(fdFile.cFileName, excludeDir) != 0)
				{
					mbaBuildFileListRecurse(fileList, sPath, includeFilePattern, excludeDir);
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
				mbNormaliseFilePath(buf,fp);
				fileList->push_back(buf);
            }
        }
    }
    while(FindNextFile(hFind, &fdFile)); //Find the next file.
    
    FindClose(hFind);
    return true;
}

void mbaFileSetWorkingDir(const std::string& path)
{
    _chdir(path.c_str());
}

std::string mbaFileGetWorkingDir()
{
    char workingDir[MB_MAX_PATH];
    return _getcwd(workingDir, sizeof(workingDir));
}

std::string	mbaFileGetAbsPath(const std::string& path)
{
	char tmp[MB_MAX_PATH];
	return _fullpath(tmp, path.c_str(), sizeof(tmp));
}

void mbaLogError(const char* str)
{
	OutputDebugString(str);
	printf("%s", str);
}

void mbaLogInfo(const char* str)
{
	OutputDebugString(str);
	printf("%s", str);
}

void mbaLogDebug(const char* str)
{
	OutputDebugString(str);
	printf("%s", str);
}

#endif
