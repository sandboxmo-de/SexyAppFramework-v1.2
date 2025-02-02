#ifndef __COMMON_H__
#define __COMMON_H__

#pragma warning(disable:4786)
#pragma warning(disable:4503)

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0400
#include <windows.h>
#include <stdlib.h>
#include <mmsystem.h>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <list>
#include <algorithm>
#include "ModVal.h"

#define LONG_BIGE_TO_NATIVE(l) (((l >> 24) & 0xFF) | ((l >> 8) & 0xFF00) | ((l << 8) & 0xFF0000) | ((l << 24) & 0xFF000000))
#define WORD_BIGE_TO_NATIVE(w) (((w >> 8) & 0xFF) | ((w << 8) & 0xFF00))
#define LONG_LITTLEE_TO_NATIVE(l) (l)
#define WORD_LITTLEE_TO_NATIVE(w) (w)

#define LENGTH(anyarray) (sizeof(anyarray) / sizeof(anyarray[0]))

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef __int64 int64;
typedef std::map<std::string, std::string> DefinesMap;

namespace Sexy
{

const ulong SEXY_RAND_MAX = 0x7FFFFFFF;

extern bool			gDebug;
extern HINSTANCE	gHInstance;

int					Rand();
int					UnsafeRand();
void				SRand(ulong theSeed);
extern std::string vformat(const char *fmt, va_list argPtr);

std::string			StrFormat(const char* fmt ...);
std::string			URLEncode(const std::string& theString);
std::string			StringToUpper(const std::string& theString);
std::string			StringToLower(const std::string& theString);
std::wstring		StringToWString(const std::string &theString);
std::string			Upper(const std::string& theData);
std::string			Lower(const std::string& theData);
std::string			Trim(const std::string& theString);
bool				StringToInt(const std::string theString, int* theIntVal);
bool				StringToDouble(const std::string theString, double* theDoubleVal);
int					StrFindNoCase(const char *theStr, const char *theFind);
bool				StrPrefixNoCase(const char *theStr, const char *thePrefix, int maxLength = 10000000);
std::string			CommaSeperate(int theValue);
std::string			Evaluate(const std::string& theString, const DefinesMap& theDefinesMap);
std::string			XMLDecodeString(const std::string& theString);
std::string			XMLEncodeString(const std::string& theString);

bool				Deltree(const std::string& thePath);
bool				FileExists(const std::string& theFileName);
void				MkDir(const std::string& theDir);
std::string			GetFileName(const std::string& thePath, bool noExtension = false);
std::string			GetFileDir(const std::string& thePath, bool withSlash = false);
std::string			RemoveTrailingSlash(const std::string& theDirectory);
std::string			AddTrailingSlash(const std::string& theDirectory, bool backSlash = false);
time_t				GetFileDate(const std::string& theFileName);
std::string			GetCurDir();
std::string			GetFullPath(const std::string& theRelPath);
std::string			GetPathFrom(const std::string& theRelPath, const std::string& theDir);


inline void			inlineUpper(std::string &theData)
{
    //std::transform(theData.begin(), theData.end(), theData.begin(), toupper);

	int aStrLen = (int) theData.length();
	for (int i = 0; i < aStrLen; i++)
	{
		theData[i] = toupper(theData[i]);
	}
}

inline void			inlineLower(std::string &theData)
{
    std::transform(theData.begin(), theData.end(), theData.begin(), tolower);
}

inline void			inlineLTrim(std::string &theData, const std::string& theChars = " \t\r\n")
{
    theData.erase(0, theData.find_first_not_of(theChars));
}

inline void			inlineRTrim(std::string &theData, const std::string& theChars = " \t\r\n")
{
    theData.resize(theData.find_last_not_of(theChars) + 1);
}

inline void			inlineTrim(std::string &theData, const std::string& theChars = " \t\r\n")
{
	inlineRTrim(theData, theChars);
	inlineLTrim(theData, theChars);
}

struct StringLessNoCase { bool operator()(const std::string &s1, const std::string &s2) const { return stricmp(s1.c_str(),s2.c_str())<0; } };

}

#endif //__COMMON_H__