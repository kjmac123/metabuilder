#include "metabuilder_pch.h"

#include "corestring.h"
#include <cstdarg>

#ifdef _MSC_VER

int core_snprintf(char * out, size_t size, const char * format, ...)
{
    va_list ap;
    va_start(ap, format);
    int result = vsprintf_s(out, size, format, ap);
    va_end(ap);
    
    return result;

}

#endif

char* core_strcpy(char* out, size_t size, const char* src)
{
    size_t len = strlen(src);
    if (len < size)
    {
        memmove(out, src, len+1);
        return out;
    }
    
    MB_ABORT();
    return NULL;
}

bool StringWildcardMatch(const char* str, int strLength, const char* pattern_, int patternLength)
{	
	const int kMaxCards = 20;

	char cards[kMaxCards][MB_MAX_PATH];
	int nCards = 0;
	char pattern[MB_MAX_PATH];
	core_strcpyfix(pattern, pattern_);

	{
		//for each separator
		char* cursor = const_cast<char*>(pattern);
		char* e;
		while ((e = strchr(cursor, '*')) != NULL)
		{
			if (nCards + 2 == kMaxCards)
			{
				MB_LOGERROR("Too many wildcards in string %s", pattern);
				mbExitError();
			}
			*e = '\0';
			core_strcpyfix(cards[nCards++], cursor);
			core_strcpyfix(cards[nCards++], "*");
			*e = '*';
			cursor = e + 1;  //skip separator
		}
		if (*cursor)
		{
			if (nCards + 1 == kMaxCards)
			{
				MB_LOGERROR("Too many wildcards in string %s", pattern);
				mbExitError();
			}
		
			core_strcpyfix(cards[nCards++], cursor);
		}
	}
	
	//When a * is encountered, look for substring match of next card
	bool match = true;
	const char* cursor = str;
	const char* card = NULL;

	enum E_MatchType
	{
		E_MatchType_MatchFirst,
		E_MatchType_MatchAny,

	} matchType = E_MatchType_MatchFirst;

	for (int i = 0; i < nCards; ++i)
	{
		card = cards[i];

		if (matchType = E_MatchType_MatchFirst)
		{
			//must match start
			const char* result = strstr(cursor, card);
			if (result != cursor)
			{
				match = false;
				break;
			}
		}
		else
		{
			if (card[0] == '*')
			{
				if (i == nCards - 1)
				{
					//Match the rest of the string. We're done.
					break;
				}
				matchType = E_MatchType_MatchAny;
			}
			else
			{
				const char* result = strstr(cursor, card);
				if (result)
				{
					cursor = result;
				}
				else
				{
					match = false;
					break;
				}
			}
		}
	}

	return match;
}
