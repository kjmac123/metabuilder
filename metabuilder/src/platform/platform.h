#ifndef PLATFORM_H
#define PLATFORM_H

#include "FilePath.h"

enum E_FileType
{
	E_FileType_Unknown,
	E_FileType_Missing,
	E_FileType_File,
	E_FileType_Dir
};

namespace Platform
{

//TODO - SAX style approach so we avoid having to store potentially large strings
struct FileInfo
{
    FileInfo()
    {
        memset(&attributes, 0, sizeof(attributes));
    }
    
	struct Attributes
	{
		bool hidden : 1;
	};

	FilePath	parentDir;
	FilePath	filename;
	FilePath	fullPath;

	E_FileType	fileType;

	Attributes	attributes;
};

typedef void(*DirWalkFileInfoFunc)(const FileInfo&, void*);

void		Init();
void		Shutdown();

bool		CreateDir(const char* osDir);
void		NormaliseFilePath(char* outFilePath, const char* inFilePath);
void		NormaliseFilePath(char* filePath);
E_FileType	GetFileType(const FilePath& filepath);
    
void        BuildFileListDir(const FilePath& dir, DirWalkFileInfoFunc fileInfoFunc, void* userdata);
void        BuildFileListFile(const FilePath& filePath, DirWalkFileInfoFunc fileInfoFunc, void* userdata);

void		FileSetWorkingDir(const std::string& path);
std::string	FileGetWorkingDir();
std::string	FileGetAbsPath(const std::string& path);
char		GetDirSep();

void		LogError(const char* str);
void		LogInfo(const char* str);
void		LogDebug(const char* str);

F64			GetSystemTicksToSecondsScale();
U64			GetSystemTicks();

}

#endif
