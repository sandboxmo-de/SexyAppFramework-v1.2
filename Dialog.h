#ifndef __DIALOG_H__
#define __DIALOG_H__

#include "Widget.h"
#include "ButtonListener.h"

namespace Sexy
{

class DialogListener;
class ButtonWidget;
class DialogButton;
class Font;

extern std::string DIALOG_YES_STRING;
extern std::string DIALOG_NO_STRING;
extern std::string DIALOG_OK_STRING;
extern std::string DIALOG_CANCEL_STRING;

typedef std::vector<std::string> StringVector;

class Dialog : public Widget, public ButtonListener
{
public:
	enum
	{
		BUTTONS_NONE,
		BUTTONS_YES_NO,
		BUTTONS_OK_CANCEL,
		BUTTONS_FOOTER
	};

	enum
	{
		ID_YES		= 1000,
		ID_NO		= 1001,
		ID_OK		= 1000,
		ID_CANCEL	= 1001,
		ID_FOOTER	= 1000
	};
	
	enum
	{
		COLOR_HEADER = 0,
		COLOR_LINES,
		COLOR_FOOTER,
		COLOR_BUTTON_TEXT,
		COLOR_BUTTON_TEXT_HILITE,
		COLOR_BKG,
		COLOR_OUTLINE,		
		NUM_COLORS
	};

	DialogListener*			mDialogListener;
	Image*					mComponentImage;	
	DialogButton*			mYesButton;
	DialogButton*			mNoButton;
	int						mNumButtons;
	std::string				mDialogHeader;
	std::string				mDialogFooter;
	std::string				mDialogLines;
	int						mButtonMode;
	Font*					mHeaderFont;
	Font*					mLinesFont;	
	int						mTextAlign;
	int						mLineSpacingOffset;
	int						mButtonHeight;
	Insets					mBackgroundInsets;
	Insets					mContentInsets;
	int						mSpaceAfterHeader;
	bool					mDragging;
	int						mDragMouseX;
	int						mDragMouseY;
	int						mId;
	bool					mIsModal;
	int						mResult;	

	int						mButtonHorzSpacing;
	int						mButtonSidePadding;
	

public:
	void					EnsureFonts();

public:
	Dialog(Image* theComponentImage, Image* theButtonComponentImage, 
		int theId, bool isModal, const std::string& theDialogHeader, const std::string& theDialogLines, const std::string& theDialogFooter, int theButtonMode);
	virtual ~Dialog();

	virtual void			SetButtonFont(Font* theFont);
	virtual void			SetHeaderFont(Font* theFont);
	virtual void			SetLinesFont(Font* theFont);

	virtual void			SetColor(int theIdx, const Color& theColor);
	virtual int				GetPreferredHeight(int theWidth);

	virtual void			Draw(Graphics* g);
	virtual void			AddedToManager(WidgetManager* theWidgetManager);
	virtual void			RemovedFromManager(WidgetManager* theWidgetManager);
	virtual void			OrderInManagerChanged();
	virtual void			Resize(int theX, int theY, int theWidth, int theHeight);

	virtual void			MouseDown(int x, int y, int theClickCount);
	virtual void			MouseDrag(int x, int y);
	virtual void			MouseUp(int x, int y);
	virtual void			Update();
	virtual	bool			IsModal();
	virtual int				WaitForResult(bool autoKill = true);

	virtual void			ButtonPress(int theId);
	virtual void			ButtonDepress(int theId);
	virtual void			ButtonDownTick(int theId);
};

}

#endif //__DIALOG_H__