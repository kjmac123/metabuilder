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
		//Cleanup
		{
			E_FileType fileType = GetFileType(dst);
			if (fileType == E_FileType_Dir)
			{
				if (RemoveDirectoryA(dst) == 0)
				{
					MB_LOGERROR("Failed to remove old dir link at %s", dst);
					return false;
				}
			}
			else if (fileType == E_FileType_File)
			{
				if (DeleteFileA(dst) == 0)
				{
					MB_LOGERROR("Failed to remove old file link at %s", dst);
					return false;
				}
			}
		}

		//Create new link
	{
		DWORD flags = 0;

		E_FileType fileType = GetFileType(src);
		if (fileType == E_FileType_Missing)
		{
			MB_LOGERROR("Failed to create link from %s to %s (source is missing)", src, dst);
			return false;
		}

		if (fileType == E_FileType_Dir)
			flags |= SYMBOLIC_LINK_FLAG_DIRECTORY;

		if (CreateSymbolicLinkA(dst, src, flags) == 0)
		{
			MB_LOGERROR("Failed to create link from %s to %s", src, dst);
			return false;
		}
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

	E_FileType GetFileType(const std::string& filepath)
	{
		DWORD fileAttr = GetFileAttributesA(filepath.c_str());
		return GetFileType(fileAttr);
	}

	void BuildFileListAddFile(
		std::vector<std::string>* fileList, 
		const FilePath& parentDir, 
		const FilePath& filename,
		const FilePath& filepath, 
		DWORD fileAttr)
	{
//		MB_LOGINFO("%s %s", parentDir.c_str(), filename.c_str());
		if (fileAttr & FILE_ATTRIBUTE_HIDDEN)
			return;

		//Ignore posix style hidden files.
		if (filename.c_str()[0] == '.')
			return;

		//Convert current-dir syntax into empty path to avoid confusing some build systems.
		const char* fp = filepath.c_str();
		
		if (filepath.GetLength() > 1)
		{
			if (fp[0] == '.' && fp[1] == '/')
			{
				fp += 2;
			}
		}
		
		fileList->push_back(fp);
	}

	static bool IgnoreSpecial(const char* filename, DWORD fileAttr)
	{
		if (strcmp(filename, ".") == 0 ||
			strcmp(filename, "..") == 0 ||
			fileAttr & FILE_ATTRIBUTE_HIDDEN)
		{
			return true;
		}

		return false;
	}

	void BuildFileListRecurse(std::vector<std::string>* fileList, const FilePath& parentDir, const FilePath& pattern)
	{
		//Recurse directories
		{
			WIN32_FIND_DATA fdFile;
			HANDLE hFind;
			FilePath filePathAndPattern;
			filePathAndPattern.Join(parentDir);
			filePathAndPattern.Join(FilePath("*"));
			if ((hFind = FindFirstFile(filePathAndPattern.c_str(), &fdFile)) == INVALID_HANDLE_VALUE)
			{
				return;
			}

			do
			{
				if (GetFileType(fdFile.dwFileAttributes) == E_FileType_Dir)
				{
					if (!IgnoreSpecial(fdFile.cFileName, fdFile.dwFileAttributes))
					{
						FilePath childFilename(fdFile.cFileName);

						FilePath childFilePath;
						childFilePath.Join(parentDir);
						childFilePath.Join(childFilename);

						BuildFileListRecurse(fileList, childFilePath, pattern);
					}
				}
			} 
			while (FindNextFile(hFind, &fdFile)); //Find the next file.

			FindClose(hFind);
		}

		{
			WIN32_FIND_DATA fdFile;
			HANDLE hFind;
			FilePath filePathAndPattern;
			filePathAndPattern.Join(parentDir);
			filePathAndPattern.Join(pattern);
			if ((hFind = FindFirstFile(filePathAndPattern.c_str(), &fdFile)) == INVALID_HANDLE_VALUE)
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

						FilePath childFilePath;
						childFilePath.Join(parentDir);
						childFilePath.Join(childFilename);

						BuildFileListAddFile(fileList, parentDir, childFilename, childFilePath, fdFile.dwFileAttributes);
					}
				}
			} 
			while (FindNextFile(hFind, &fdFile)); //Find the next file.

			FindClose(hFind);
		}
	}

	void BuildFileList(std::vector<std::string>* fileList, const char* filePatternOrDir)
	{
		DWORD fileAttr = GetFileAttributesA(filePatternOrDir);
		E_FileType fileType = GetFileType(fileAttr);
		if (fileType == E_FileType_File)
		{
			FilePath fp(filePatternOrDir);

			FilePath dir;
			FilePath filename;
			fp.SplitDirFilename(&dir, &filename);

			BuildFileListAddFile(fileList, dir, filename, FilePath(filePatternOrDir), fileAttr);
		}
		else
		{
			FilePath dir;
			FilePath pattern;
			if (fileType == E_FileType_Dir)
			{
				dir = FilePath(filePatternOrDir);
			}
			else
			{
				FilePath fp(filePatternOrDir);
				fp.SplitDirFilename(&dir, &pattern);
			}
			BuildFileListRecurse(fileList, dir, pattern);
		}
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

