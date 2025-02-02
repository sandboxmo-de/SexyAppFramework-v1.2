#include "DialogButton.h"
#include "SysFont.h"
#include "WidgetManager.h"

using namespace Sexy;

static int gDialogButtonColors[][3] = {
	{255, 255, 255},
	{255, 255, 255}};

DialogButton::DialogButton(Image* theComponentImage, int theId, ButtonListener* theListener) :
	ButtonWidget(theId, theListener)
{
	mComponentImage = theComponentImage;

	mTextOffsetX = mTextOffsetY = 0;
	mTranslateX = mTranslateY = 1;
	mDoFinger = true;

	SetColors(gDialogButtonColors, NUM_COLORS);
}

void DialogButton::Draw(Graphics* g)
{
	if (mBtnNoDraw)
		return;

	if (mComponentImage==NULL)
	{
		ButtonWidget::Draw(g);
		return;
	}

	if ((mFont == NULL) && (mLabel.length() > 0))
		mFont = new SysFont(mWidgetManager->mApp, "SansSerif", 12, true);

	bool doTranslate = IsButtonDown();

	if (mNormalRect.mWidth==0)
	{
		if (doTranslate)
			g->Translate(mTranslateX, mTranslateY);

		g->DrawImageBox(Rect(0, 0, mWidth, mHeight), mComponentImage);
	}
	else
	{
		if (mDisabled && (mDisabledRect.mWidth > 0) && (mDisabledRect.mHeight > 0))
			g->DrawImageBox(mDisabledRect, Rect(0, 0, mWidth, mHeight), mComponentImage);
		else if (IsButtonDown())	
			g->DrawImageBox(mDownRect, Rect(0, 0, mWidth, mHeight), mComponentImage);
		else if(mIsOver)
			g->DrawImageBox(mOverRect, Rect(0, 0, mWidth, mHeight), mComponentImage);
		else
			g->DrawImageBox(mNormalRect, Rect(0, 0, mWidth, mHeight), mComponentImage);
		
		if (doTranslate)
			g->Translate(mTranslateX, mTranslateY);
	}

	g->SetColorizeImages(true);

	if (mIsOver)
		g->SetColor(mColors[COLOR_LABEL_HILITE]);
	else
		g->SetColor(mColors[COLOR_LABEL]);

	if (mFont != NULL)
	{
		g->SetFont(mFont);		
		
		int aFontX = (mWidth - mFont->StringWidth(mLabel))/2;
		int aFontY = (mHeight + mFont->GetAscent() - mFont->GetAscentPadding() - mFont->GetAscent()/6 - 1)/2;

		g->DrawString(mLabel, aFontX + mTextOffsetX, aFontY + mTextOffsetY);
	}

	g->SetColorizeImages(false);

	if (doTranslate)
		g->Translate(-1, -1);
}
