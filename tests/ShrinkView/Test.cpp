// test.cpp

#include <stdio.h>

#include <Alert.h>
#include <Application.h>
#include <Bitmap.h>
#include <File.h>
#include <Resources.h>
#include <Window.h>

#include <PaneSwitch.h>
#include <ShrinkView.h>

#define TEST_MESSAGE 'test'



class MyWindow : public BWindow {
private:
	PaneSwitch* fPermissionsSwitch;
public:

MyWindow(void) :
	BWindow(BRect(100, 100, 400, 400), "PaneSwitch test", B_TITLED_WINDOW, B_ASYNCHRONOUS_CONTROLS)
{
	BRect b = Bounds();
	
	BView *back = new ShrinkView(BRect(b.left, b.top + 1, b.right, b.top + 40), "First");
	b = back->Frame();
		
	AddChild(back);
	
	AddChild(new ShrinkView(BRect(b.left, b.bottom + 50, b.right, b.bottom + 97), "Second"));

	fPermissionsSwitch = new PaneSwitch(BRect(20,20,160,160), "Permissions");
	fPermissionsSwitch->SetMessage(new BMessage(TEST_MESSAGE));
	back->AddChild(fPermissionsSwitch);

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

MyApp(void) : BApplication("application/libwalter-PaneSwitch-test")
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

