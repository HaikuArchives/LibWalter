// test.cpp

#include <stdio.h>

#include <Alert.h>
#include <Application.h>
#include <Bitmap.h>
#include <File.h>
#include <Resources.h>
#include <Window.h>

#include <PaneSwitch.h>
#include <DialogPane.h>
#include <BarberPole.h>


#define TEST_MESSAGE 'test'

class MyWindow : public BWindow { // --------------------------------------------------------------
private:
	DialogPane* fMoreOptionsPane;
	PaneSwitch* fLatch;
public:

MyWindow(void) :
	BWindow(BRect(100, 100, 400, 300), "DiaglogPane test", B_TITLED_WINDOW, B_NOT_ZOOMABLE )
{
	fMoreOptionsPane = new DialogPane(BRect(0,0,300,200), BRect(0,200,300,300),0,
		               "moreOptions", B_FOLLOW_LEFT | B_FOLLOW_BOTTOM);

	AddChild(fMoreOptionsPane);

	fMoreOptionsPane->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	fMoreOptionsPane->SetLowColor(ui_color(B_PANEL_BACKGROUND_COLOR));


	fLatch = new PaneSwitch(BRect(05,75,30,90), "moreOptionsLatch", true, B_FOLLOW_BOTTOM | B_FOLLOW_LEFT);
	fMoreOptionsPane->AddChild(fLatch);
		
	fLatch->SetTarget(fMoreOptionsPane);	
	fMoreOptionsPane->SetSwitch(fLatch);	
	
	fMoreOptionsPane->AddChild(new BarberPole(BRect(10,10,150,30),"barber", B_FOLLOW_TOP | B_FOLLOW_LEFT , B_WILL_DRAW));

}


void MessageReceived(BMessage *message)
{
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

MyApp(void) : BApplication("application/libwalter-DialogPane-test")
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

