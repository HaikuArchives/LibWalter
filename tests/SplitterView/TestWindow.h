#ifndef _TestWindow_
#define _TestWindow_

#include <Window.h>
#include <Message.h>
#include <Bitmap.h>

#include "MainView.h"
#include "SplitterView.h"

class TestWindow : public BWindow
{
public:
	/* Constructors & Destructor*/
	TestWindow ();

	virtual void MessageReceived(BMessage *message);
	virtual bool QuitRequested(void);
	
private:

	MainView *mainView;
	SplitterView *splitView;
	BBitmap *frontBitmap;
};




#endif
