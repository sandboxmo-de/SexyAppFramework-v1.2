#include "Debug.h"

#include "AutoCrit.h"
#include "CritSect.h"

#include <time.h>

#include "memmgr.h"

extern bool gInAssert = false;

static FILE *gTraceFile = NULL;
static int gTraceFileLen = 0;
static int gTraceFileNum = 1;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
struct SEXY_ALLOC_INFO
{
	int		size;
	char	file[_MAX_PATH+1];
	int		line;
};
static bool gShowLeaks = false;
static bool gSexyAllocMapValid = false;
class SexyAllocMap : public std::map<void*,SEXY_ALLOC_INFO>
{
public:
	CritSect mCrit;

public:
	SexyAllocMap() { gSexyAllocMapValid = true; }
	~SexyAllocMap() 
	{ 
		if (gShowLeaks) 
			SexyDumpUnfreed();		

		gSexyAllocMapValid = false; 
	}
};
static SexyAllocMap gSexyAllocMap;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SexyTrace(const char *theStr)
{
	if (gTraceFile==NULL)
	{
		gTraceFileNum = (gTraceFileNum+1)%2;
		char aBuf[50];
		sprintf(aBuf,"trace%d.txt",gTraceFileNum+1);
		gTraceFile = fopen(aBuf,"w");
		if (gTraceFile==NULL)
			return;
	}

	fprintf(gTraceFile,"%s\n",theStr);
	fflush(gTraceFile);

	gTraceFileLen += strlen(theStr);
	if (gTraceFileLen > 100000)
	{
		fclose(gTraceFile);
		gTraceFile = NULL;
		gTraceFileLen = 0;
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void SexyTraceFmt(const char* fmt ...)
{
	// Does not append a newline by default, also takes vararg parameters

	va_list argList;
	va_start(argList, fmt);
	std::string result = vformat(fmt, argList);
	va_end(argList);

	
	if (gTraceFile==NULL)
	{
		gTraceFileNum = (gTraceFileNum+1)%2;
		char aBuf[50];
		sprintf(aBuf,"trace%d.txt",gTraceFileNum+1);
		gTraceFile = fopen(aBuf,"w");
		if (gTraceFile==NULL)
			return;
	}

	fprintf(gTraceFile,"%s",result.c_str());
	fflush(gTraceFile);

	gTraceFileLen += result.length();
	if (gTraceFileLen > 100000)
	{
		fclose(gTraceFile);
		gTraceFile = NULL;
		gTraceFileLen = 0;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SexyMemAddTrack(void *addr,  int asize,  const char* fname, int lnum)
{
	if (!gSexyAllocMapValid)
		return;

	AutoCrit aCrit(gSexyAllocMap.mCrit);
	gShowLeaks = true;

	SEXY_ALLOC_INFO &info = gSexyAllocMap[addr];
	strncpy(info.file, fname, sizeof(info.file)-1);
	info.line = lnum;
	info.size = asize;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SexyMemRemoveTrack(void* addr)
{
	if (!gSexyAllocMapValid)
		return;

	AutoCrit aCrit(gSexyAllocMap.mCrit);
	SexyAllocMap::iterator anItr = gSexyAllocMap.find(addr);
	if (anItr != gSexyAllocMap.end())
		gSexyAllocMap.erase(anItr);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SexyDumpUnfreed()
{
	if (!gSexyAllocMapValid)
		return;

	AutoCrit aCrit(gSexyAllocMap.mCrit);
	SexyAllocMap::iterator i;
	int totalSize = 0;
	char buf[8192];

	FILE* f = fopen("mem_leaks.txt", "wt");
	if (!f)
		return;

	time_t aTime = time(NULL);
	sprintf(buf, "Memory Leak Report for %s\n",	asctime(localtime(&aTime)));
	fprintf(f, buf);
	OutputDebugString("\n");
	OutputDebugString(buf);
	for(i = gSexyAllocMap.begin(); i != gSexyAllocMap.end(); i++) 
	{
		sprintf(buf, "%s(%d) : Leak %d byte%s\n", i->second.file, i->second.line, i->second.size,i->second.size>1?"s":"");
		OutputDebugString(buf);
		fprintf(f, buf);
		totalSize += i->second.size;
	}


	sprintf(buf, "-----------------------------------------------------------\n");
	fprintf(f, buf);
	OutputDebugString(buf);
	sprintf(buf, "Total Unfreed: %d bytes\n\n", totalSize);
	OutputDebugString(buf);
	fprintf(f, buf);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void OutputDebug(const char* fmt ...)
{
	va_list argList;
    va_start(argList, fmt);
    std::string result = vformat(fmt, argList);
    va_end(argList);

	OutputDebugString(result.c_str());
}

