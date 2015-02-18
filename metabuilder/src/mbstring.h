#ifndef MBSTRING_H
#define MBSTRING_H

class MBString
{
public:
	MBString();
	MBString(const MBString& rhs);
	explicit MBString(const char* str);
	explicit MBString(const std::string& str);

	MBString& operator=(const MBString& rhs);

	const char* c_str() const;

private:
	std::string m_storage;
};

#endif
