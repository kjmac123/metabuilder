#ifndef PLATFORM_H
#define PLATFORM_H

//Maximum path length is a complete mess. Let's go with 512bytes for now.
//stackoverflow.com/questions/833291/is-there-an-equivalent-to-winapis-max-path-under-linux-unix
#define MB_MAX_PATH 512

enum E_FileType
{
	E_FileType_Unknown,
	E_FileType_Missing,
	E_FileType_File,
	E_FileType_Dir
};

namespace Platform
{

void		Init();
void		Shutdown();

bool		CreateDir(const char* osDir);
bool		CreateLink(const char* src, const char* dst);
void		NormaliseFilePath(char* outFilePath, const char* inFilePath);
void		NormaliseFilePath(char* filePath);
E_FileType	GetFileType(const std::string& filepath);
bool		BuildFileListRecurse(std::vector<std::string>* fileList, const char* osInputDir, const char* includeFilePattern, const char* excludeDir);

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
