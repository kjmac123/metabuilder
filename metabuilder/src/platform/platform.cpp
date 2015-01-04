#include "metabuilder_pch.h"

#include "platform.h"

namespace Platform
{
	void NormaliseFilePath(char* outFilePath, const char* inFilePath)
	{
		strcpy(outFilePath, inFilePath);
		NormaliseFilePath(outFilePath);
	}
	
	void NormaliseFilePath(char* filePath)
	{
		bool preceedingSlash = false;

		for (char* cursor = filePath; *cursor; ++cursor)
		{
			char c = *cursor;
			//Normalise slashes
			if (c == '\\' || c == '/')
				c = Platform::GetDirSep();

			//Ignore duplicate slashes
			if (c == Platform::GetDirSep())
			{
				if (preceedingSlash)
					continue;
				preceedingSlash = true;
			}
			else
			{
				preceedingSlash = false;
			}

			*cursor = c;
		}
	}
}
