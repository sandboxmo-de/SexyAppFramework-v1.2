#ifndef __PROPERTIESPARSER_H__
#define __PROPERTIESPARSER_H__

#include "SexyAppBase.h"

namespace Sexy
{

class XMLParser;

class PropertiesParser
{
public:
	SexyAppBase*			mApp;
	XMLParser*				mXMLParser;
	std::string				mError;
	bool					mHasFailed;

protected:
	void					Fail(const std::string& theErrorText);

	bool					ParseSingleElement(std::string* theString);
	bool					ParseStringArray(StringVector* theStringVector);
	bool					ParseProperties();

	bool					DoParseProperties();

public:
	PropertiesParser(SexyAppBase* theApp);
	virtual ~PropertiesParser();

	bool					ParsePropertiesFile(const std::string& theFilename);
	bool					ParsePropertiesBuffer(const Buffer& theBuffer);
	std::string				GetErrorText();
};

}

#endif //__PROPERTIESPARSER_H__


