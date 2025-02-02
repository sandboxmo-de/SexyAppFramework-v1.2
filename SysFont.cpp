#include "SysFont.h"
#include "DDImage.h"
#include "SexyAppBase.h"
#include "Graphics.h"
#include "ImageFont.h"
#include "MemoryImage.h"
#include "D3DInterface.h"
#include "WidgetManager.h"

using namespace Sexy;

SysFont::SysFont(const std::string& theFace, int thePointSize, bool bold, bool italics, bool underline)
{
	Init(gSexyAppBase,theFace,thePointSize,ANSI_CHARSET,bold,italics,underline,false);
}

SysFont::SysFont(SexyAppBase* theApp, const std::string& theFace, int thePointSize, int theScript, bool bold, bool italics, bool underline)
{
	Init(theApp,theFace,thePointSize,theScript,bold,italics,underline,true);
}

void SysFont::Init(SexyAppBase* theApp, const std::string& theFace, int thePointSize, int theScript, bool bold, bool italics, bool underline, bool useDevCaps)
{
	mApp = theApp;

	HDC aDC = ::GetDC(mApp->mHWnd);

	int aHeight = -MulDiv(thePointSize, useDevCaps?GetDeviceCaps(aDC, LOGPIXELSY):96, 72);

	mHFont = CreateFont(aHeight, 0, 0, 0, bold ? FW_BOLD : FW_NORMAL, italics, underline,
			false, theScript, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
			DEFAULT_PITCH | FF_DONTCARE, theFace.c_str());
	
	TEXTMETRIC aTextMetric;	
	HFONT anOldFont = (HFONT) SelectObject(aDC, mHFont);
	GetTextMetrics(aDC, &aTextMetric);
	SelectObject(aDC, anOldFont);
	ReleaseDC(mApp->mHWnd, aDC);

	mHeight = aTextMetric.tmHeight;
	mAscent = aTextMetric.tmAscent;	
}

SysFont::SysFont(const SysFont& theSysFont)
{
	LOGFONT aLogFont;

	GetObject(theSysFont.mHFont, sizeof(LOGFONT), &aLogFont);
	mHFont = CreateFontIndirect(&aLogFont);
	mApp = theSysFont.mApp;
	mHeight = theSysFont.mHeight;
	mAscent = theSysFont.mAscent;
}

SysFont::~SysFont()
{
	DeleteObject(mHFont);
}

ImageFont* SysFont::CreateImageFont()
{
	int i;
	MemoryImage*			anImage;
	int anImageCharWidth, anImageXOff, anImageYOff;

	////////////////////////////////////////////////////
	// Step 1: Create image 
	anImageCharWidth = CharWidth('W')*2;
	anImageXOff = anImageCharWidth/4;
	anImageYOff = mHeight/2;
	int aWidth = 257*anImageCharWidth;
	int aHeight = mHeight*2;

	BITMAPINFO aBitmapInfo;
	memset(&aBitmapInfo,0,sizeof(aBitmapInfo));
	BITMAPINFOHEADER &aHeader = aBitmapInfo.bmiHeader;
	aHeader.biSize = sizeof(aHeader);
	aHeader.biWidth = aWidth;
	aHeader.biHeight = -aHeight;
	aHeader.biPlanes = 1;
	aHeader.biBitCount = 32;
	aHeader.biCompression = BI_RGB;

	HDC aDC = CreateCompatibleDC(NULL);

	DWORD *aBits = NULL;
	HBITMAP aBitmap = CreateDIBSection(aDC,&aBitmapInfo,DIB_RGB_COLORS,(void**)&aBits,NULL,0);

	HBITMAP anOldBitmap = (HBITMAP)SelectObject(aDC,aBitmap);
	HFONT anOldFont = (HFONT)SelectObject(aDC,mHFont);

	HBRUSH anOldBrush = (HBRUSH)SelectObject(aDC,GetStockObject(BLACK_BRUSH));
	Rectangle(aDC,0,0,aWidth,aHeight);

	SetBkMode(aDC, TRANSPARENT); 	
	SetTextColor(aDC, RGB(255,255,255));
				
	int xpos = anImageXOff;
	int ypos = anImageYOff;
	for (i=0; i<256; i++)
	{
		char aChar = i;
		TextOut(aDC,xpos,ypos,&aChar,1);
		xpos += anImageCharWidth;
	}
	GdiFlush();


	SelectObject(aDC,anOldBrush);
	SelectObject(aDC,anOldBitmap);
	SelectObject(aDC,anOldFont);

	int aSize = aWidth*aHeight;
	anImage = new MemoryImage(mApp);
	anImage->Create(aWidth,aHeight);
	DWORD *src = aBits;
	DWORD *dst = anImage->GetBits();
	for (i=0; i<aSize; i++)
	{
		DWORD anAlpha = ((*src++)&0xff)<<24;
		*dst++ = anAlpha | 0xFFFFFF;
	}
	anImage->BitsChanged();
	DeleteObject(aBitmap);


	////////////////////////////////////////////////////
	// Step 2: Create image font

	ImageFont *aFont = new ImageFont(anImage);
	FontLayer *aFontLayer = &aFont->mFontData->mFontLayerList.back();

	aFontLayer->mAscent = mAscent;
	aFontLayer->mHeight = mHeight;

	for (i=0; i<256; i++)
	{
		char aChar = i;

		aFontLayer->mCharData[(uchar) aChar].mImageRect = Rect(aChar*anImageCharWidth,0,anImageCharWidth,anImage->mHeight);
		aFontLayer->mCharData[(uchar) aChar].mWidth = CharWidth(aChar);
		aFontLayer->mCharData[(uchar) aChar].mOffset = Point(-anImageXOff,-anImageYOff);
	}

	aFont->GenerateActiveFontLayers();
	aFont->mActiveListValid = true;

	return aFont;
}

int	SysFont::StringWidth(const std::string& theString)
{
	HDC aDC = ::GetDC(mApp->mHWnd);
	HFONT anOldFont = (HFONT) SelectObject(aDC, mHFont);
	RECT aRect = {0, 0, 0, 0};	
	DrawTextEx(aDC, (char*) theString.c_str(), theString.length(), &aRect, DT_CALCRECT | DT_NOPREFIX, NULL);
	SelectObject(aDC, anOldFont);
	ReleaseDC(mApp->mHWnd, aDC);

	return aRect.right;
}

void SysFont::DrawString(Graphics* g, int theX, int theY, const std::string& theString, const Color& theColor, const Rect& theClipRect)
{
	DDImage* aDDImage = dynamic_cast<DDImage*>(g->mDestImage);

	if (aDDImage != NULL)
	{
		LPDIRECTDRAWSURFACE aSurface = aDDImage->GetSurface();
		if (aSurface != NULL)
		{
			HDC aDC;

			if (aDDImage->mLockCount > 0)
				aDDImage->mSurface->Unlock(NULL);

			if ((g->mDestImage == gSexyAppBase->mWidgetManager->mImage) && (gSexyAppBase->Is3DAccelerated()))
				gSexyAppBase->mDDInterface->mD3DInterface->Flush();				
			
			if (aSurface->GetDC(&aDC) == DD_OK)
			{				
				HFONT anOldFont = (HFONT) SelectObject(aDC, mHFont);
				
				SetBkMode(aDC, TRANSPARENT); 
				
				IntersectClipRect(aDC, theClipRect.mX, theClipRect.mY, theClipRect.mX + theClipRect.mWidth, theClipRect.mY + theClipRect.mHeight);
				
				SetTextColor(aDC, RGB(theColor.GetRed(), theColor.GetGreen(), theColor.GetBlue()));
				
				TextOut(aDC, theX + g->mTransX, theY - mAscent + 1 + g->mTransY, theString.c_str(), theString.length());
				
				SelectObject(aDC, anOldFont);
				
				aSurface->ReleaseDC(aDC);

				aDDImage->DeleteAllNonSurfaceData();				
			}			

			if (aDDImage->mLockCount > 0)
				aDDImage->mSurface->Lock(NULL, &aDDImage->mLockedSurfaceDesc, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);			
		}
	}
}

Font* SysFont::Duplicate()
{
	return new SysFont(*this);
}
