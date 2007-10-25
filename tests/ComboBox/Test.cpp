// test.cpp

#include <stdio.h>

#include <Alert.h>
#include <Application.h>
#include <Bitmap.h>
#include <File.h>
#include <Resources.h>
#include <Window.h>

#include "ComboBox.h"

#define TEST_MESSAGE 'test'

class MyWindow : public BWindow { // --------------------------------------------------------------
private:
	ComboBox* combo;
public:

MyWindow(void) :
	BWindow(BRect(100, 100, 400, 400), "ComboBox test", B_TITLED_WINDOW, B_ASYNCHRONOUS_CONTROLS)
{
	BView *back = new BView(Bounds(), NULL, B_FOLLOW_ALL_SIDES, 0);
	AddChild(back);
	
	combo = new ComboBox(BRect(10,10,260,260), "Combo test", 
	                                           "Combo test label", 
	                                           "initial text", new BMessage(TEST_MESSAGE));

	combo->AddItem("item number 1");
	combo->AddItem("item number 2");
	combo->AddItem("item number 3");
	combo->AddItem("item number 4");			
	back->AddChild(combo);

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

MyApp(void) : BApplication("application/libwalter-ComboBox-test")
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

