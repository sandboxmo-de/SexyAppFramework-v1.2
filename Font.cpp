#include "Font.h"
#include "Image.h"

using namespace Sexy;

Font::Font()
{	
	mAscent = 0;
	mHeight = 0;
	mAscentPadding = 0;
	mLineSpacingOffset = 0;
}

Font::Font(const Font& theFont) :
	mAscent(theFont.mAscent),
	mHeight(theFont.mHeight),
	mAscentPadding(theFont.mAscentPadding),
	mLineSpacingOffset(theFont.mLineSpacingOffset)
{
}

Font::~Font()
{
}

int	Font::GetAscent()
{
	return mAscent;
}

int	Font::GetAscentPadding()
{
	return mAscentPadding;
}

int	Font::GetDescent()
{
	return mHeight - mAscent;
}

int	Font::GetHeight()
{
	return mHeight;
}

int Font::GetLineSpacingOffset()
{
	return mLineSpacingOffset;
}

int Font::GetLineSpacing()
{
	return mHeight + mLineSpacingOffset;
}

int	Font::StringWidth(const std::string& theString)
{
	return 0;
}

int Font::CharWidth(char theChar)
{
	char aString[] = {theChar, '\0'};
	return StringWidth(aString);
}

int Font::CharWidthKern(char theChar, char thePrevChar)
{
	return CharWidth(theChar);
}

void Font::DrawString(Graphics* g, int theX, int theY, const std::string& theString, const Color& theColor, const Rect& theClipRect)
{
}