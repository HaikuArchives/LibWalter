#ifndef _TestWindow_
#define _TestWindow_

#include <Window.h>
#include <Message.h>
#include <Bitmap.h>

#include "MainView.h"
#include "BViewSplitter.h"

class TestWindow : public BWindow
{
public:
	/* Constructors & Destructor*/
	TestWindow ();

	virtual void MessageReceived(BMessage *message);
	virtual bool QuitRequested(void);
	
private:

	MainView *mainView;
	BViewSplitter *splitView;
	BBitmap *frontBitmap;
};




#endif
