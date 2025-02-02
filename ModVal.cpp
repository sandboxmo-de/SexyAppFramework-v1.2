#include "ModVal.h"
#include "Common.h"
#include <fstream>

struct ModStorage
{		
	bool					mHasData;
	int						mInt; 
	double					mDouble;	
};

// An individual area (0 - 9, for M() - M9(), per file)
struct ModStorageArea
{
	std::vector<ModStorage> mMemVector; // Stores values	
};

// All deferred mod records per file
struct FileMods
{
	const char*				mFileNamePtr;
	bool					mReparsed;
	ModStorageArea			mModStorageArea[10]; // 10 storage areas per file
};

typedef std::map<std::string, FileMods> StringToFileModsMap;

static StringToFileModsMap gFileModsMap;
static bool gAlreadyGaveError = false;
time_t gLastFileTime = 0;

static FileMods* CreateFileMods(const char* theFileName)
{	
	StringToFileModsMap::iterator aFileModsItr = gFileModsMap.find(theFileName);
	if (aFileModsItr != gFileModsMap.end())
		return &aFileModsItr->second;

	// This is a new file
	FileMods aNewFileMods;
	aNewFileMods.mFileNamePtr = theFileName; 
	aNewFileMods.mReparsed = false;
	aFileModsItr = gFileModsMap.insert(StringToFileModsMap::value_type(theFileName+2, aNewFileMods)).first;

	FileMods* aFileMods = &aFileModsItr->second;

	// Change this thinggie
	DWORD anOldProtect;
	VirtualProtect((LPVOID) theFileName, 5, PAGE_READWRITE, &anOldProtect);
	*((char*) theFileName) = 0;
	*((FileMods**) (theFileName+1)) = aFileMods;
	VirtualProtect((LPVOID) theFileName, 5, anOldProtect, &anOldProtect);
	
	return aFileMods;	
}

static void SetModValue(int theAreaNum, const char* theFileName, int theLineNum, int theInt, double theDouble)
{
	if (*theFileName != 0)
		CreateFileMods(theFileName);	
	FileMods* aFileMods = *((FileMods**) (theFileName+1));

	ModStorageArea* aModStorageArea = &aFileMods->mModStorageArea[theAreaNum];
	if (theLineNum >= (int) aModStorageArea->mMemVector.size())
	{
		ModStorage aModStorage;
		aModStorage.mHasData = false;
		aModStorage.mInt = 0;
		aModStorage.mDouble = 0.0;
		aModStorageArea->mMemVector.resize(theLineNum+1, aModStorage);
	}

	ModStorage* aModStorage = &aModStorageArea->mMemVector[theLineNum];
	aModStorage->mHasData = true;
	aModStorage->mInt = theInt;
	aModStorage->mDouble = theDouble;
}


int Sexy::ModVal(int theAreaNum, const char* theFileName, int theLineNum, int theInt)
{	
	if (*theFileName != 0)
		CreateFileMods(theFileName);	
	FileMods* aFileMods = *((FileMods**) (theFileName+1));
			
	ModStorageArea* aModStorageArea = &aFileMods->mModStorageArea[theAreaNum];
	if (theLineNum >= (int) aModStorageArea->mMemVector.size())
	{
		return theInt;
	}

	ModStorage* aModStorage = &aModStorageArea->mMemVector[theLineNum];
	return aModStorage->mInt;
}

double Sexy::ModVal(int theAreaNum, const char* theFileName, int theLineNum, double theDouble)
{
	if (*theFileName != 0)
		CreateFileMods(theFileName);	
	FileMods* aFileMods = *((FileMods**) (theFileName+1));
			
	ModStorageArea* aModStorageArea = &aFileMods->mModStorageArea[theAreaNum];
	if (theLineNum >= (int) aModStorageArea->mMemVector.size())
	{
		return theDouble;
	}

	ModStorage* aModStorage = &aModStorageArea->mMemVector[theLineNum];
	return aModStorage->mDouble;
}

float Sexy::ModVal(int theAreaNum, const char* theFileName, int theLineNum, float theFloat)
{
	return (float) ModVal(theAreaNum, theFileName, theLineNum, (double) theFloat);
}

static bool ModStringToInteger(const char* theString, int* theIntVal)
{
	*theIntVal = 0;

	int theRadix = 10;
	bool isNeg = false;

	unsigned i = 0;
	if (theString[i] == '-')
	{
		isNeg = true;
		i++;
	}

	for (;;)
	{
		char aChar = theString[i];
		
		if ((theRadix == 10) && (aChar >= '0') && (aChar <= '9'))
			*theIntVal = (*theIntVal * 10) + (aChar - '0');
		else if ((theRadix == 0x10) && 
			(((aChar >= '0') && (aChar <= '9')) || 
			 ((aChar >= 'A') && (aChar <= 'F')) || 
			 ((aChar >= 'a') && (aChar <= 'f'))))
		{			
			if (aChar <= '9')
				*theIntVal = (*theIntVal * 0x10) + (aChar - '0');
			else if (aChar <= 'F')
				*theIntVal = (*theIntVal * 0x10) + (aChar - 'A') + 0x0A;
			else
				*theIntVal = (*theIntVal * 0x10) + (aChar - 'a') + 0x0A;
		}
		else if (((aChar == 'x') || (aChar == 'X')) && (i == 1) && (*theIntVal == 0))
		{
			theRadix = 0x10;
		}
		else if (aChar == ')')
		{
			if (isNeg)
				*theIntVal = -*theIntVal;
			return true;
		}
		else
		{
			*theIntVal = 0;
			return false;
		}

		i++;
	}		
}

static bool ModStringToDouble(const char* theString, double* theDoubleVal)
{
	*theDoubleVal = 0.0;

	bool isNeg = false;

	unsigned i = 0;
	if (theString[i] == '-')
	{
		isNeg = true;
		i++;
	}

	for (;;)
	{
		char aChar = theString[i];

		if ((aChar >= '0') && (aChar <= '9'))
			*theDoubleVal = (*theDoubleVal * 10) + (aChar - '0');
		else if (aChar == '.')
		{
			i++;
			break;
		}		
		else if ((aChar == ')') || ((aChar == 'f') && (theString[i+1] == ')'))) // At end
		{
			if (isNeg)
				*theDoubleVal = -*theDoubleVal;
			return true;
		}
		else
		{
			*theDoubleVal = 0.0;
			return false;
		}

		i++;
	}

	double aMult = 0.1;
	for (;;)
	{
		char aChar = theString[i];

		if ((aChar >= '0') && (aChar <= '9'))
		{
			*theDoubleVal += (aChar - '0') * aMult;	
			aMult /= 10.0;
		}
		else if ((aChar == ')') || ((aChar == 'f') && (theString[i+1] == ')'))) // At end
		{
			if (isNeg)
				*theDoubleVal = -*theDoubleVal;
			return true;
		}
		else
		{
			*theDoubleVal = 0.0;
			return false;
		}

		i++;
	}
}

bool Sexy::ReparseModValues()
{
	gAlreadyGaveError = false;	

	if (gLastFileTime == 0)
	{
		char anEXEName[256];
		GetModuleFileName(NULL, anEXEName, 256);
		gLastFileTime = GetFileDate(anEXEName);
	}

	bool hasNewFiles = false;
	std::string aFileList;

	// Process each file one at a time
	StringToFileModsMap::iterator aFileModItr = gFileModsMap.begin();
	while (aFileModItr != gFileModsMap.end())
	{
		// Clear out the old deferred doohickeys
		FileMods* aFileMod = &aFileModItr->second;
		for (int i = 0; i < 10; i++)
			aFileMod->mModStorageArea[i].mMemVector.clear();
		aFileMod->mReparsed = true;

		std::string aFileName = aFileModItr->first;

		time_t aThisTime = GetFileDate(aFileName);
		if (aThisTime > gLastFileTime)
		{
			gLastFileTime = aThisTime;
			hasNewFiles = true;
		}

		if (aFileList.length() > 0)
			aFileList += "\r\n  ";
		aFileList += aFileName;

		int aLineNum = 1;

		std::fstream aStream(aFileName.c_str(), std::ios::in);
		if (aStream.is_open())
		{
			while (!aStream.eof())
			{
				bool areaUsed[10] = {false};
				char aString[512];
				aStream.getline(aString, 512);

				int aCharIdx = 0;
				while (aString[aCharIdx] != 0)
				{
					if (aString[aCharIdx] == '(')
					{
						int theAreaNum = -1;

						if ((aCharIdx >= 2) && (aString[aCharIdx-1] == 'M') &&
							(!isalpha((unsigned char) aString[aCharIdx-2])))
						{
							theAreaNum = 0;							
						}
						else if ((aCharIdx >= 3) && 
							(aString[aCharIdx-1] >= '1') && (aString[aCharIdx-1] <= '9') &&
							(aString[aCharIdx-2] == 'M') &&
							(!isalpha((unsigned char) aString[aCharIdx-3])))
						{
							theAreaNum = aString[aCharIdx-1] - '0';
						}

						if (theAreaNum != -1)
						{
							// Try to parse out a number

							int anIntVal = 0;
							double aDoubleVal = 0.0;

							if ((ModStringToInteger(aString + aCharIdx + 1, &anIntVal)) ||
								(ModStringToDouble(aString + aCharIdx + 1, &aDoubleVal)))
							{
								if (!areaUsed[theAreaNum])
								{
								
									// We found a mod value!
									SetModValue(theAreaNum, aFileMod->mFileNamePtr, aLineNum, anIntVal, aDoubleVal);

									areaUsed[theAreaNum] = true;
								}								
								else
								{
									char aStr[512];
									sprintf(aStr, "ERROR in %s on line %d: If you want to use more than one M() value per line, you must use the numbered macros (ie: M1(), M2(), M3(), etc), "
										"using each one a maximum of once per line.  Alternatively, you may split up your statement into multiple lines.", aFileName.c_str(), aLineNum);
									MessageBox(NULL, aStr, "MODVAL ERROR", MB_OK | MB_ICONERROR);
									return false;
								}
							}							
						}
					}

					aCharIdx++;
				}

				aLineNum++;
			}
		}
		else
		{
			MessageBox(NULL, (std::string("ERROR: Unable to open ") + aFileName + " for reparsing.").c_str(), "MODVAL ERROR!", MB_OK | MB_ICONERROR);
			return false;
		}		

		++aFileModItr;
	}

	if (!hasNewFiles)
	{
		if (aFileList.length() == 0)
			aFileList = "none";
		MessageBox(NULL, (std::string("WARNING: No file changes detected.  Files parsed: \r\n  ") + aFileList).c_str(), "MODVAL WARNING!", MB_OK | MB_ICONWARNING);
		return false;
	}

	return true;
}
