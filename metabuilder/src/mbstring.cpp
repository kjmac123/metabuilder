#include "metabuilder_pch.h"

#include "mbstring.h"

MBString::MBString()
{
}

MBString::MBString(const MBString& rhs)
{
	*this = rhs;
}

MBString::MBString(const char* str)
{
	m_storage = str;
}

MBString::MBString(const std::string& str)
{
	m_storage = str;
}

MBString& MBString::operator=(const MBString& rhs)
{
	m_storage = rhs.m_storage;

	return *this;
}

const char* MBString::c_str() const
{
	return m_storage.c_str();
}
