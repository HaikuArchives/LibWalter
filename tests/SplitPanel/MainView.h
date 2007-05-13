#ifndef _MainView_
#define _MainView_

#include <View.h>
#include <Cursor.h>

class MainView : public BView
{
public:
	MainView(BRect rect, const char *name)	;
//	virtual void MouseMoved(BPoint point, uint32 transit, const  BMessage *message);
//	virtual void Draw(BRect rect);
	virtual void MessageReceived(BMessage *message);
	virtual void AttachedToWindow();
//	virtual void MouseDown(BPoint point);	
//	virtual void MouseUp(BPoint point);
//	virtual void FrameResized(float width, float height);
//	void SetTopView(BView *topView);
//	void DrawRect(BRect rect);

private:
//	BView *topView;
BButton *myButton;
//	BBitmap *aBitmap;
};

#endif // _MainView_
