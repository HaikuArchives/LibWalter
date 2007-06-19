#ifndef _LayeringTestApp_
#define _LayeringTestApp_


#include <Window.h>
#include <Application.h>
#include "TestWindow.h"


/**
 *	Dies ist ein Test-Kommentar
 *
 *
 */


class LayeringTestApp : public BApplication 
{
public:
	/* Constructors & Destructor*/
	LayeringTestApp ();

	/* Hooks */

	virtual bool QuitRequested(void);
	virtual void MessageReceived(BMessage* message);
	TestWindow 		*testWindow;
};


#endif
