#ifndef __TEXTWIDGET_H__
#define __TEXTWIDGET_H__

#include "Widget.h"
#include "ScrollListener.h"

namespace Sexy
{

class ScrollbarWidget;
class Font;

typedef std::vector<std::string> StringVector;
typedef std::vector<int> IntVector;

class TextWidget : public Widget, public ScrollListener
{
public:
	Font*				mFont;
	ScrollbarWidget*	mScrollbar;		
	
	StringVector		mLogicalLines;
	StringVector		mPhysicalLines;	
	IntVector			mLineMap;
	//Vector				mLineMap = new Vector();	
	double				mPosition;
	double				mPageSize;	
	bool				mStickToBottom;	
	int					mHiliteArea[2][2];
	int					mMaxLines;
	
public:
	TextWidget();

	virtual StringVector GetLines();
	virtual void SetLines(StringVector theNewLines);	
	virtual void Clear();		
	virtual void DrawColorString(Graphics* g, const std::string& theString, int x, int y, bool useColors);			
	virtual void DrawColorStringHilited(Graphics* g, const std::string& theString, int x, int y, int theStartPos, int theEndPos);		
	virtual int GetStringIndex(const std::string& theString, int thePixel);
	
	virtual int GetColorStringWidth(const std::string& theString);		
	virtual void Resize(int theX, int theY, int theWidth, int theHeight);		
	virtual Color GetLastColor(const std::string& theString);		
	virtual void AddToPhysicalLines(int theIdx, const std::string& theLine);
	virtual void AddLine(const std::string& theString);		
	virtual bool SelectionReversed();		
	virtual void GetSelectedIndices(int theLineIdx, int* theIndices);		
	virtual void Draw(Graphics* g);
	virtual void ScrollPosition(int theId, double thePosition);		
	virtual void GetTextIndexAt(int x, int y, int* thePosArray);
	virtual std::string GetSelection();		

	virtual void MouseDown(int x, int y, int theClickCount);		
	virtual void MouseDrag(int x, int y);	
	
	virtual void KeyDown(KeyCode theKey);
};

}

#endif //__TEXTWIDGET_H__