#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef PLATFORM_POSIX
#define MB_MAX_PATH PATH_MAX
#elif PLATFORM_WINDOWS
#define MB_MAX_PATH 260
#else
#define MB_MAX_PATH 260
#endif

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
