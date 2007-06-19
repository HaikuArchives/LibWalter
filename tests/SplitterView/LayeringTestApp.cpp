#ifndef _LayeringTestApp_
#include "LayeringTestApp.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <Entry.h>
#include "TestWindow.h"

/**
 *   Blablabal
 *
 *
 */


LayeringTestApp::LayeringTestApp() : BApplication("application/x-va.VideoEditor")
{
	testWindow = new TestWindow();
	testWindow->Show();
	


}

bool
LayeringTestApp::QuitRequested(void)
{
	printf("Quit requested!\n");
	return true;
}

void
LayeringTestApp::MessageReceived(BMessage* message) {

	BApplication::MessageReceived(message);
}
