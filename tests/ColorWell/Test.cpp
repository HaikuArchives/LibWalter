// test.cpp

#include <stdio.h>

#include <Alert.h>
#include <Application.h>
#include <Bitmap.h>
#include <File.h>
#include <Resources.h>
#include <Window.h>

#include "ColorWell.h"

#define TEST_MESSAGE 'test'

class MyWindow : public BWindow { // --------------------------------------------------------------
private:
	ColorWell* color;
public:

MyWindow(void) :
	BWindow(BRect(100, 100, 400, 400), "ColorWell test", B_TITLED_WINDOW, B_ASYNCHRONOUS_CONTROLS)
{
	BView *back = new BView(Bounds(), NULL, B_FOLLOW_ALL_SIDES, 0);
	AddChild(back);
	
	color = new ColorWell(back->Bounds(), "Color", new BMessage(TEST_MESSAGE));
	color->SetStyle(COLORWELL_ROUND_WELL);
	back->AddChild(color);
	ResizeTo(color->Frame().Width(), color->Frame().Height());
}


void MessageReceived(BMessage *message)
{
	if (message->what == TEST_MESSAGE)
		(new BAlert("Hello!", "Hello!", "OK"))->Go();
	BWindow::MessageReceived(message);
}

bool QuitRequested(void)
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return BWindow::QuitRequested();
}

}; // MyWindow ------------------------------------------------------------------------------------


class MyApp : public BApplication { // ------------------------------------------------------------
private:
	MyWindow *fWindow;
public:

MyApp(void) : BApplication("application/libwlater-general-test")
{
	fWindow = new MyWindow();
}

void ReadyToRun(void)
{
	fWindow->Show();
}

}; // MyApp ---------------------------------------------------------------------------------------

int main(int, char**)
{
	(new MyApp())->Run();
	return 0;
}

