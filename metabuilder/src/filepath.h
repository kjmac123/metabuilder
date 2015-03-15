#ifndef FILEPATH_H
#define FILEPATH_H

class FilePath
{
public:
	FilePath();
	FilePath(const FilePath& rhs);
	explicit FilePath(const std::string& str);
	explicit FilePath(const char* str);

	FilePath& operator=(const FilePath& rhs);

	const char* c_str() const;

	int GetLength() const;

	void GetDir(FilePath* outDir) const;
	bool GetFilename(FilePath* outFilename) const;
	void GetFileExtension(FilePath* outExt) const;

	bool SplitLast(FilePath* result1, FilePath* result2) const;

	void Join(const FilePath& rhs);

private:
	void Normalise() const;

	mutable std::string m_storage;
	mutable bool		m_normalised;
};
/*
class FilePathPosix
{

};

class FilePathWindows
{

};
*/

#endif
