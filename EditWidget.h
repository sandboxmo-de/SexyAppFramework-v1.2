#ifndef __EDITWIDGET_H__
#define __EDITWIDGET_H__

#include "Widget.h"

namespace Sexy
{

class Font;
class EditListener;

class EditWidget : public Widget
{
public:
	enum
	{
		COLOR_BKG,
		COLOR_OUTLINE,
		COLOR_TEXT,
		COLOR_HILITE,
		COLOR_HILITE_TEXT,
		NUM_COLORS
	};

	int						mId;
	std::string				mString;
	std::string				mPasswordDisplayString;
	Font*					mFont;

	struct WidthCheck
	{
		Font *mFont;
		int mWidth;
	};
	typedef std::list<WidthCheck> WidthCheckList;
	WidthCheckList				mWidthCheckList;
	
	EditListener*			mEditListener;		
	bool					mShowingCursor;
	bool					mDrawSelOverride; // set this to true to draw selected text even when not in focus
	int						mCursorPos;
	int						mHilitePos;
	int						mBlinkAcc;
	int						mBlinkDelay;
	int						mLeftPos;		
	int						mMaxChars;
	int						mMaxPixels;
	char					mPasswordChar;
	
	std::string				mUndoString;
	int						mUndoCursor;
	int						mUndoHilitePos;
	int						mLastModifyIdx;

protected:
	virtual void			ProcessKey(KeyCode theKey, char theChar);
	std::string&			GetDisplayString();

public:
	virtual void			SetFont(Font* theFont, Font* theWidthCheckFont = NULL);
	virtual void			SetText(const std::string& theText, bool leftPosToZero = true);
	virtual bool			IsPartOfWord(char theChar);
	virtual int				GetCharAt(int x, int y);

	virtual void			Resize(int theX, int theY, int theWidth, int theHeight);
	virtual void			Draw(Graphics* g); // Already translated;

	virtual void			Update();
	virtual void			MarkDirty();

	virtual bool			WantsFocus();
	virtual void			GotFocus();
	virtual void			LostFocus();
	virtual void			FocusCursor(bool bigJump);

	virtual void			KeyDown(KeyCode theKey);
	virtual void			KeyChar(char theChar);

	virtual void			MouseDown(int x, int y, int theClickCount);
	virtual void			MouseUp(int x, int y, int theClickCount);
	virtual void			MouseDrag(int x, int y);
	virtual void			MouseEnter();
	virtual void			MouseLeave();
	void					ClearWidthCheckFonts();
	void					AddWidthCheckFont(Font *theFont, int theMaxPixels = -1); // defaults to mMaxPixels
	void					EnforceMaxPixels();

public:
	EditWidget(int theId, EditListener* theEditListener);
	virtual ~EditWidget();
};

}

#endif //__EDITWIDGET_H__