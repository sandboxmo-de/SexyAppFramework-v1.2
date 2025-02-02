#include "PropertiesParser.h"
#include "XMLParser.h"

using namespace Sexy;

PropertiesParser::PropertiesParser(SexyAppBase* theApp)
{
	mApp = theApp;
	mHasFailed = false;
	mXMLParser = NULL;
}

void PropertiesParser::Fail(const std::string& theErrorText)
{
	if (!mHasFailed)
	{
		mHasFailed = true;
		int aLineNum = mXMLParser->GetCurrentLineNum();

		char aLineNumStr[16];
		sprintf(aLineNumStr, "%d", aLineNum);	

		mError = theErrorText;

		if (aLineNum > 0)
			mError += std::string(" on Line ") + aLineNumStr;

		if (mXMLParser->GetFileName().length() > 0)
			mError += " in File '" + mXMLParser->GetFileName() + "'";
	}
}


PropertiesParser::~PropertiesParser()
{
}

bool PropertiesParser::ParseSingleElement(std::string* aString)
{
	*aString = "";

	for (;;)
	{
		XMLElement aXMLElement;
		if (!mXMLParser->NextElement(&aXMLElement))
			return false;
		
		if (aXMLElement.mType == XMLElement::TYPE_START)
		{
			Fail("Unexpected Section: '" + aXMLElement.mValue + "'");
			return false;			
		}
		else if (aXMLElement.mType == XMLElement::TYPE_ELEMENT)
		{
			*aString = aXMLElement.mValue;
		}		
		else if (aXMLElement.mType == XMLElement::TYPE_END)
		{
			return true;
		}
	}
}

bool PropertiesParser::ParseStringArray(StringVector* theStringVector)
{
	theStringVector->clear();

	for (;;)
	{
		XMLElement aXMLElement;
		if (!mXMLParser->NextElement(&aXMLElement))
			return false;
		
		if (aXMLElement.mType == XMLElement::TYPE_START)
		{
			if (aXMLElement.mValue == "String")
			{
				std::string aString;

				if (!ParseSingleElement(&aString))
					return false;

				theStringVector->push_back(aString);
			}
			else
			{
				Fail("Invalid Section '" + aXMLElement.mValue + "'");
				return false;
			}
		}
		else if (aXMLElement.mType == XMLElement::TYPE_ELEMENT)
		{
			Fail("Element Not Expected '" + aXMLElement.mValue + "'");
			return false;
		}		
		else if (aXMLElement.mType == XMLElement::TYPE_END)
		{
			return true;
		}
	}
}

bool PropertiesParser::ParseProperties()
{
	for (;;)
	{
		XMLElement aXMLElement;
		if (!mXMLParser->NextElement(&aXMLElement))
			return false;
		
		if (aXMLElement.mType == XMLElement::TYPE_START)
		{
			if (aXMLElement.mValue == "String")
			{
				std::string aDef;

				if (!ParseSingleElement(&aDef))
					return false;

				std::string anId = aXMLElement.mAttributes["id"];

				mApp->SetString(anId, aDef);
			}
			else if (aXMLElement.mValue == "StringArray")
			{
				StringVector aDef;

				if (!ParseStringArray(&aDef))
					return false;

				std::string anId = aXMLElement.mAttributes["id"];

				mApp->mStringVectorProperties.insert(StringStringVectorMap::value_type(anId, aDef));
			}
			else if (aXMLElement.mValue == "Boolean")
			{
				std::string aVal;

				if (!ParseSingleElement(&aVal))
					return false;

				aVal = Upper(aVal);

				bool boolVal;
				if ((aVal == "1") || (aVal == "YES") || (aVal == "ON") || (aVal == "TRUE"))
					boolVal = true;
				else if ((aVal == "0") || (aVal == "NO") || (aVal == "OFF") || (aVal == "FALSE"))
					boolVal = false;
				else
				{
					Fail("Invalid Boolean Value: '" + aVal + "'");
					return false;
				}

				std::string anId = aXMLElement.mAttributes["id"];

				mApp->SetBoolean(anId, boolVal);
			}
			else if (aXMLElement.mValue == "Integer")
			{
				std::string aVal;

				if (!ParseSingleElement(&aVal))
					return false;

				int anInt;
				if (!StringToInt(aVal, &anInt))
				{
					Fail("Invalid Integer Value: '" + aVal + "'");
					return false;
				}

				std::string anId = aXMLElement.mAttributes["id"];

				mApp->SetInteger(anId, anInt);
			}
			else if (aXMLElement.mValue == "Double")
			{
				std::string aVal;

				if (!ParseSingleElement(&aVal))
					return false;

				double aDouble;
				if (!StringToDouble(aVal, &aDouble))
				{
					Fail("Invalid Double Value: '" + aVal + "'");
					return false;
				}

				std::string anId = aXMLElement.mAttributes["id"];

				mApp->SetDouble(anId, aDouble);
			}
			else
			{
				Fail("Invalid Section '" + aXMLElement.mValue + "'");
				return false;
			}
		}
		else if (aXMLElement.mType == XMLElement::TYPE_ELEMENT)
		{
			Fail("Element Not Expected '" + aXMLElement.mValue + "'");
			return false;
		}		
		else if (aXMLElement.mType == XMLElement::TYPE_END)
		{
			return true;
		}
	}
}

bool PropertiesParser::DoParseProperties()
{
	if (!mXMLParser->HasFailed())
	{
		for (;;)
		{
			XMLElement aXMLElement;
			if (!mXMLParser->NextElement(&aXMLElement))
				break;

			if (aXMLElement.mType == XMLElement::TYPE_START)
			{
				if (aXMLElement.mValue == "Properties")
				{
					if (!ParseProperties())
						break;
				}
				else 
				{
					Fail("Invalid Section '" + aXMLElement.mValue + "'");
					break;
				}
			}
			else if (aXMLElement.mType == XMLElement::TYPE_ELEMENT)
			{
				Fail("Element Not Expected '" + aXMLElement.mValue + "'");
				break;
			}
		}
	}

	if (mXMLParser->HasFailed())
		Fail(mXMLParser->GetErrorText());	

	delete mXMLParser;
	mXMLParser = NULL;

	return !mHasFailed;
}

bool PropertiesParser::ParsePropertiesBuffer(const Buffer& theBuffer)
{
	mXMLParser = new XMLParser();

	std::string aString;
	aString.insert(aString.begin(), (char*)theBuffer.GetDataPtr(), (char*)theBuffer.GetDataPtr() + theBuffer.GetDataLen());
	mXMLParser->SetStringSource(aString);
	return DoParseProperties();
}

bool PropertiesParser::ParsePropertiesFile(const std::string& theFilename)
{
	mXMLParser = new XMLParser();
	mXMLParser->OpenFile(theFilename);
	return DoParseProperties();	
}

std::string PropertiesParser::GetErrorText()
{
	return mError;
}