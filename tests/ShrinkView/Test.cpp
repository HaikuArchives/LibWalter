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
	ShrinkView*	fBack;
public:

MyWindow(void) :
	BWindow(BRect(100, 100, 400, 400), "ShrinkView test", B_TITLED_WINDOW, B_ASYNCHRONOUS_CONTROLS)
{
	BRect b = Bounds();

	fBack = new ShrinkView(BRect(b.left, b.top + 1, b.right, b.top + 70), "First");
	b = fBack->Frame();

	AddChild(fBack);

	AddChild(new ShrinkView(BRect(b.left, b.bottom + 70, b.right, b.bottom + 100), "Second"));

	fPermissionsSwitch = new PaneSwitch(BRect(20, 20, 60, 60), "Permissions");
	fPermissionsSwitch->SetMessage(new BMessage(TEST_MESSAGE));
	fBack->AddChild(fPermissionsSwitch);

}


void MessageReceived(BMessage *message)
{
	if (message->what == TEST_MESSAGE) {
		switch ((new BAlert("Hello", "Changing mode", "Auto fit", "Normal"))->Go()) {
			case 0:
				fBack->SetMode(ShrinkView::B_AUTO_FIT_WINDOW);
				break;
			case 1:
				fBack->SetMode(ShrinkView::B_NORMAL);
				break;
		}

	}
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

	MyWindow*	fWindow;


public:

MyApp(void) : BApplication("application/libwalter-ShrinkView-test")
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

