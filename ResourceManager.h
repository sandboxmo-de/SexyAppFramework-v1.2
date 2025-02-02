#ifndef __SEXY_RESOURCEMANAGER_H__
#define __SEXY_RESOURCEMANAGER_H__

#include "Common.h"
#include "Image.h"
#include "SexyAppBase.h"
#include <string>
#include <map>

namespace ImageLib
{
class Image;
};

namespace Sexy
{

class XMLParser;
class XMLElement;
class Image;
class SoundInstance;
class SexyAppBase;
class Font;

typedef std::map<std::string, std::string>	StringToStringMap;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ResourceManager
{
protected:

	struct BaseRes
	{
		std::string mResGroup;
		std::string mPath;
		StringToStringMap mXMLAttributes;
		bool mFromProgram;

		virtual ~BaseRes() {}
		virtual void DeleteResource() { }
	};

	struct ImageRes : public BaseRes
	{
		SharedImageRef mImage;
		std::string mAlphaImage;
		std::string mAlphaGridImage;
		std::string mVariant;
		bool mAutoFindAlpha;
		bool mPalletize;
		bool mA4R4G4B4;
		bool mA8R8G8B8;
		bool mDDSurface;
		bool mPurgeBits;
		bool mMinimizeSubdivisions;
		int mRows;
		int mCols;	
		int mTotal;	
		DWORD mAlphaColor;
		AnimInfo mAnimInfo;

		virtual void DeleteResource();
	};

	struct SoundRes : public BaseRes
	{
		int mSoundId;
		double mVolume;
		int mPanning;

		virtual void DeleteResource();
	};

	struct FontRes : public BaseRes
	{
		Font *mFont;
		Image *mImage;
		std::string mImagePath;
		std::string mTags;

		virtual void DeleteResource();
	};

	typedef std::map<std::string,BaseRes*> ResMap;
	std::set<std::string,StringLessNoCase> mLoadedGroups;

	ResMap					mImageMap;
	ResMap					mSoundMap;
	ResMap					mFontMap;
	ResMap::iterator		mImageItr;
	ResMap::iterator		mSoundItr;
	ResMap::iterator		mFontItr;
	int						mCurSoundId;

	XMLParser*				mXMLParser;
	std::string				mError;
	bool					mHasFailed;
	SexyAppBase*			mApp;
	std::string				mCurResGroup;
	std::string				mDefaultPath;
	std::string				mDefaultIdPrefix;
	bool					mAllowMissingProgramResources;


	bool					Fail(const std::string& theErrorText);

	bool					ParseCommonResource(XMLElement &theElement, BaseRes *theRes, ResMap &theMap);
	bool					ParseSoundResource(XMLElement &theElement);
	bool					ParseImageResource(XMLElement &theElement);
	bool					ParseFontResource(XMLElement &theElement);
	bool					ParseSetDefaults(XMLElement &theElement);
	bool					ParseResources();

	bool					DoParseResources();
	void					DeleteMap(ResMap &theMap);
	virtual void			DeleteResources(ResMap &theMap, const std::string &theGroup);

	bool					LoadAlphaGridImage(ImageRes *theRes, DDImage *theImage);
	bool					LoadAlphaImage(ImageRes *theRes, DDImage *theImage);
	virtual bool			DoLoadImage(ImageRes *theRes);
	virtual bool			DoLoadFont(FontRes* theRes);

	int						GetNumResources(const std::string &theGroup, ResMap &theMap);

public:
	ResourceManager(SexyAppBase *theApp);
	virtual ~ResourceManager();

	bool					ParseResourcesFile(const std::string& theFilename);
	std::string				GetErrorText();
	bool					HadError();
	bool					IsGroupLoaded(const std::string &theGroup);

	int						GetNumImages(const std::string &theGroup);
	int						GetNumSounds(const std::string &theGroup);
	int						GetNumFonts(const std::string &theGroup);
	int						GetNumResources(const std::string &theGroup);

	virtual bool			LoadNextImage();
	virtual bool			LoadNextSound();
	virtual bool			LoadNextFont();
	virtual bool			LoadNextResource();

	virtual void			StartLoadResources(const std::string &theGroup);
	virtual bool			LoadResources(const std::string &theGroup);

	bool					ReplaceImage(const std::string &theId, Image *theImage);
	bool					ReplaceSound(const std::string &theId, int theSound);
	bool					ReplaceFont(const std::string &theId, Font *theFont);

	void					DeleteImage(const std::string &theName);
	SharedImageRef			LoadImage(const std::string &theName);

	SharedImageRef			GetImage(const std::string &theId);
	int						GetSound(const std::string &theId);
	Font*					GetFont(const std::string &theId);
	
	// Returns all the XML attributes associated with the image
	const StringToStringMap&	GetImageAttributes(const std::string &theId);

	// These throw a ResourceManagerException if the resource is not found
	virtual SharedImageRef	GetImageThrow(const std::string &theId);
	virtual int				GetSoundThrow(const std::string &theId);
	virtual Font*			GetFontThrow(const std::string &theId);

	void					SetAllowMissingProgramImages(bool allow);

	virtual void			DeleteResources(const std::string &theGroup);
	void					DeleteExtraImageBuffers(const std::string &theGroup);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
struct ResourceManagerException : public std::exception
{
	std::string what;
	ResourceManagerException(const std::string &theWhat) : what(theWhat) { }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void ReadIntVector(const std::string &theVal, std::vector<int> &theVector)
{
	theVector.clear();

	std::string::size_type aPos = 0;
	while (true)
	{
		theVector.push_back(atoi(theVal.c_str()+aPos));
		aPos = theVal.find_first_of(',',aPos);
		if (aPos==std::string::npos)
			break;

		aPos++;
	}	
};

}

#endif //__PROPERTIESPARSER_H__


