#ifndef __LISTWIDGET_H__
#define __LISTWIDGET_H__

#include "ScrollListener.h"
#include "Widget.h"

namespace Sexy 
{

typedef std::vector<std::string> StringVector;
typedef std::vector<Color> ColorVector;

class ScrollbarWidget;
class ListListener;
class Font;

class ListWidget : public Widget, public ScrollListener 
{
public:
	enum 
	{
		JUSTIFY_LEFT			=0,
		JUSTIFY_CENTER,
		JUSTIFY_RIGHT
	};

	enum 
	{
		COLOR_BKG				=0,
		COLOR_OUTLINE,
		COLOR_TEXT,
		COLOR_HILITE,
		COLOR_SELECT,
		COLOR_SELECT_TEXT,
	};

public:
	int							mId;	
	Font*						mFont;
	ScrollbarWidget*			mScrollbar;
	int							mJustify;

	StringVector				mLines;
	ColorVector					mLineColors;
	double						mPosition;	
	double						mPageSize;
	int							mHiliteIdx;
	int							mSelectIdx;
	ListListener*				mListListener;		
	ListWidget*					mParent;
	ListWidget*					mChild;
	bool						mSortFromChild;		
	bool						mDrawOutline;
	int							mMaxNumericPlaces;
	int							mItemHeight;

	bool						mDrawSelectWhenHilited;
	bool						mDoFingerWhenHilited;

	void						SetHilite(int theHiliteIdx, bool notifyListener = false);

public:
	ListWidget(int theId, Font *theFont, ListListener *theListListener);
	virtual ~ListWidget();

	virtual void				RemovedFromManager(WidgetManager *theManager);

	virtual std::string			GetSortKey(int theIdx);
	virtual void				Sort(bool ascending);
	virtual std::string			GetStringAt(int theIdx);
	virtual void				Resize(int theX, int theY, int theWidth, int theHeight);
	virtual int					AddLine(const std::string& theLine, bool alphabetical);
	virtual void				SetLine(int theIdx, const std::string& theString);
	virtual int					GetLineCount();
	virtual int					GetLineIdx(const std::string& theLine);	
	virtual void				SetColor(const std::string& theLine, const Color& theColor);
	virtual void				SetColor(int theIdx, const Color& theColor);
	virtual void				RemoveLine(int theIdx);
	virtual void				RemoveAll();
	virtual int					GetOptimalWidth();
	virtual int					GetOptimalHeight();
	virtual void				OrderInManagerChanged();
	virtual void				Draw(Graphics *g);
	virtual void				ScrollPosition(int theId, double thePosition);
	virtual void				MouseMove(int x, int y);
	virtual void				MouseWheel(int theDelta);
	virtual void				MouseDown(int x, int y, int theClickCount);
	virtual void				MouseLeave();
	virtual void				SetSelect(int theSelectIdx);
};

}

#endif // __LISTWIDGET_H__
