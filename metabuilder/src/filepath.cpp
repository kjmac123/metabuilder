#include "core.h"
#include "filepath.h"

FilePath::FilePath()
{
	m_normalised = false;
}

FilePath::FilePath(const FilePath& rhs)
{
	*this = rhs;
}

FilePath::FilePath(const std::string& str)
{
	m_storage = str;
	m_normalised = false;
}

FilePath::FilePath(const char* str)
{
	m_storage = str;
	m_normalised = false;
}

FilePath& FilePath::operator=(const FilePath& rhs)
{
	m_storage = rhs.m_storage;
	m_normalised = rhs.m_normalised;

	return *this;
}

const char* FilePath::c_str() const
{
	Normalise();
	return m_storage.c_str();
}

int FilePath::GetLength() const
{
	return static_cast<int>(m_storage.length());
}

void FilePath::GetDir(FilePath* outDir) const
{
	Normalise();

	*outDir = FilePath();

	size_t len = m_storage.length();
	const char* chars = m_storage.c_str();
	for (int i = len - 1; i >= 0; --i)
	{
		if (chars[i] == '/')
		{
			chars += i + 1;
			*outDir = FilePath(chars);
			outDir->m_normalised = true;
			break;
		}
	}

}

void FilePath::GetFilename(FilePath* outFilename) const
{
	Normalise();

	size_t len = m_storage.length();
	const char* chars = m_storage.c_str();
	for (int i = len - 1; i >= 0; --i)
	{
		if (chars[i] == '/')
		{
			chars += i + 1;
			*outFilename = FilePath(chars);
			outFilename->m_normalised = true;
			break;
		}
	}
}

void FilePath::GetFileExtension(FilePath* outExt) const
{
	*outExt = FilePath();

	size_t len = m_storage.length();
	const char* chars = m_storage.c_str();
	for (int i = len - 1; i >= 0; --i)
	{
		if (chars[i] == '.')
		{
			chars += i + 1;
			*outExt = FilePath(chars);
			break;
		}
	}
}

void FilePath::SplitDirFilename(FilePath* outDir, FilePath* outFilename) const
{
	Normalise();

	size_t len = m_storage.length();
	const char* chars = m_storage.c_str();
	for (int i = len - 1; i >= 0; --i)
	{
		if (chars[i] == '/')
		{
			char& c = const_cast<char&>(chars[i]);
			c = '\0';
			*outDir = FilePath(m_storage);
			outDir->m_normalised = true;
			c = '/';

			*outFilename = FilePath(chars+i+1);
			outFilename->m_normalised = true;
			break;
		}
	}
}

void FilePath::Join(const FilePath& rhs)
{
	Normalise();
	rhs.Normalise();

	size_t len = m_storage.length();
	bool trailingSlash = len > 0 && m_storage[len - 1] == '/';
	size_t rhsLen = rhs.m_storage.length();
	bool leadingSlash = rhsLen > 0 && rhs.m_storage[rhsLen - 1] == '/';

	//Copy only one slash if both leading and trailing
	if (leadingSlash && trailingSlash)
	{
		char buf[MB_MAX_PATH];
		memcpy(buf, m_storage.c_str(), len - 1);
		memcpy(buf + len, rhs.m_storage.c_str(), rhsLen);
	}
	//Join together without adding new slash
	else if (leadingSlash || trailingSlash)
	{
		m_storage += rhs.m_storage;
	}
	else
	{
		//No slash needed if our length is 0
		if (len == 0)
		{
			m_storage = rhs.m_storage;
		}
		else
		{
			//Add new slash
			char buf[MB_MAX_PATH];
			sprintf(buf, "%s/%s", m_storage.c_str(), rhs.m_storage.c_str());
			m_storage = buf;
		}
	}
}

void FilePath::Normalise() const
{
	if (!m_normalised)
	{
		size_t len = m_storage.length();
		for (size_t i = 0; i < len; ++i)
		{
			char& c = m_storage[i];
			if (c == '\\')
			{
				c = '/';
			}
		}

		m_normalised = true;
	}
}
