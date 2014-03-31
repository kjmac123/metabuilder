#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef PLATFORM_OSX
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

bool		_mbaCreateDir(const char* osDir);
bool		mbaCreateLink(const char* src, const char* dst);
void		mbaNormaliseFilePath(char* outFilePath, const char* inFilePath);
E_FileType	mbaGetFileType(const std::string& filepath);
bool		mbaBuildFileListRecurse(std::vector<std::string>* fileList, const char* osInputDir, const char* includeFilePattern, const char* excludeDir);

void		mbaFileSetWorkingDir(const std::string& path);
std::string	mbaFileGetWorkingDir();
std::string	mbaFileGetAbsPath(const std::string& path);

void		mbaLogError(const char* str);
void		mbaLogInfo(const char* str);
void		mbaLogDebug(const char* str);

#endif
