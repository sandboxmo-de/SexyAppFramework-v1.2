#include "Slider.h"
#include "Graphics.h"
#include "Image.h"
#include "SliderListener.h"
#include "WidgetManager.h"
#include "SexyAppBase.h"

using namespace Sexy;

Slider::Slider(Image* theTrackImage, Image* theThumbImage, int theId, SliderListener* theListener) : 		
	mTrackImage(theTrackImage),
	mThumbImage(theThumbImage),
	mId(theId),
	mListener(theListener),
	mVal(0.0)
{
	mDragging = false;
}

void Slider::SetValue(double theValue)
{	
	mVal = theValue;
	MarkDirtyFull();
}

bool Slider::HasTransparencies()
{
	return true;
}

void Slider::Draw(Graphics* g)
{	
	if (mTrackImage != NULL)
	{
		int cw = mTrackImage->GetWidth()/3;
		int ch = mTrackImage->GetHeight();

		int ty = (mHeight - ch) / 2;

		g->DrawImage(mTrackImage, 0, ty, Rect(0, 0, cw, ch));

		Graphics aClipG(*g);
		aClipG.ClipRect(cw, ty, mWidth - cw*2, ch);
		for (int i = 0; i < (mWidth-cw*2+cw-1)/cw; i++)
			aClipG.DrawImage(mTrackImage, cw + i*cw, ty, Rect(cw, 0, cw, ch));

		g->DrawImage(mTrackImage, mWidth-cw, ty, Rect(cw*2, 0, cw, ch));
	}

	//g->SetColor(Color(255, 255, 0));
	//g->FillRect(0, 0, mWidth, mHeight);

	g->DrawImage(mThumbImage, (int) (mVal * (mWidth - mThumbImage->GetWidth())), (mHeight - mThumbImage->GetHeight()) / 2);		
}

void Slider::MouseDown(int x, int y, int theClickCount)
{
	int aThumbX = (int) (mVal * (mWidth - mThumbImage->GetWidth()));

	if ((x >= aThumbX) && (x < aThumbX + mThumbImage->GetWidth()))
	{
		mWidgetManager->mApp->SetCursor(CURSOR_DRAGGING);
		mDragging = true;
		mRelX = x - aThumbX;
	}
}

void Slider::MouseMove(int x, int y)
{
	int aThumbX = (int) (mVal * (mWidth - mThumbImage->GetWidth()));

	if ((x >= aThumbX) && (x < aThumbX + mThumbImage->GetWidth()))
		mWidgetManager->mApp->SetCursor(CURSOR_DRAGGING);
	else
		mWidgetManager->mApp->SetCursor(CURSOR_POINTER);
}

void Slider::MouseDrag(int x, int y)
{
	if (mDragging)
	{	
		double anOldVal = mVal;

		mVal = (x - mRelX) / (double) (mWidth - mThumbImage->GetWidth());
		if (mVal < 0.0)
			mVal = 0.0;
		if (mVal > 1.0)
			mVal = 1.0;

		if (mVal != anOldVal)
		{
			mListener->SliderVal(mId, mVal);
			MarkDirtyFull();
		}
	}
}

void Slider::MouseUp(int x, int y)
{
	mDragging = false;
	mWidgetManager->mApp->SetCursor(CURSOR_POINTER);
	mListener->SliderVal(mId, mVal);
}

void Slider::MouseLeave()
{
	if (!mDragging)
		mWidgetManager->mApp->SetCursor(CURSOR_POINTER);
}