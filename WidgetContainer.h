#ifndef __WIDGETCONTAINER_H__
#define __WIDGETCONTAINER_H__

#include "Common.h"
#include "Rect.h"
#include "Flags.h"

namespace Sexy
{

class Graphics;
class Widget;
class WidgetManager;

typedef std::list<Widget*> WidgetList;


class WidgetContainer
{
public:
	WidgetList				mWidgets;
	WidgetManager*			mWidgetManager;
	WidgetContainer*		mParent;

	bool					mUpdateThisRemoved;
	WidgetList::iterator	mUpdateIterator;
	ulong					mLastWMUpdateCount;
	int						mUpdateCnt;
	bool					mDirty;
	int						mX;
	int						mY;
	int						mWidth;
	int						mHeight;
	bool					mHasAlpha;
	bool					mClip;
	FlagsMod				mWidgetFlagsMod;
	int						mPriority;

public:	
	Widget*					GetWidgetAtHelper(int x, int y, int theFlags, bool* found, int* theWidgetX, int* theWidgetY);
	bool					IsBelowHelper(Widget* theWidget1, Widget* theWidget2, bool* found);

public:	
	WidgetContainer();
	virtual ~WidgetContainer();

	virtual Rect			GetRect();
	virtual bool			Intersects(WidgetContainer* theWidget);	

	void					AddWidget(Widget* theWidget);
	void					RemoveWidget(Widget* theWidget);	
	bool					HasWidget(Widget* theWidget);	
	void					DisableWidget(Widget* theWidget);
	void					RemoveAllWidgets(bool doDelete = false, bool recursive = false);
	
	void					SetFocus(Widget* theWidget);
	bool					IsBelow(Widget* theWidget1, Widget* theWidget2);			
	void					MarkAllDirty();
	void					BringToFront(Widget* theWidget);
	void					BringToBack(Widget* theWidget);
	void					PutBehind(Widget* theWidget, Widget* theRefWidget);
	void					PutInfront(Widget* theWidget, Widget* theRefWidget);
	Point					GetAbsPos(); // relative to top level

	virtual void			MarkDirty();
	virtual void			MarkDirtyFull();
	virtual void			MarkDirtyFull(WidgetContainer* theWidget);
	virtual void			MarkDirty(WidgetContainer* theWidget);

	virtual void			AddedToManager(WidgetManager* theWidgetManager);
	virtual void			RemovedFromManager(WidgetManager* theWidgetManager);			

	virtual void			Update();
	virtual void			UpdateAll(ModalFlags* theFlags);
	virtual void			UpdateF(float theFrac);
	virtual void			UpdateFAll(ModalFlags* theFlags, float theFrac);
	virtual void			Draw(Graphics* g);
	virtual void			DrawAll(ModalFlags* theFlags, Graphics* g);	
};

};

#endif //__WIDGETCONTAINER_H__
