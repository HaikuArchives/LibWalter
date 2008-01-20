#include "TestWindow.h"

#include <TextControl.h>
#include <Box.h>
#include <Font.h>
#include <Button.h>
#include <StringView.h>
#include <TranslationUtils.h>
#include <stdio.h>
#include <Application.h>

TestWindow::TestWindow()
 : BWindow(BRect(100,100,500,600),"LayeringWindow",B_TITLED_WINDOW_LOOK ,B_NORMAL_WINDOW_FEEL,  B_ASYNCHRONOUS_CONTROLS , B_CURRENT_WORKSPACE)
{

	mainView = new MainView(BRect(0,0,400,500),"blabla");
	MainView *mainView2 =  new MainView(BRect(0,0,400,500),"blabla");
	MainView *mainView3 =  new MainView(BRect(0,0,400,500),"blabla");
	splitView = new SplitterView(Bounds(),B_HORIZONTAL, B_FOLLOW_ALL, B_WILL_DRAW);
	splitView->AddChild(mainView);
	splitView->AddChild(mainView2);
	splitView->AddChild(mainView3);

	AddChild(splitView);
	splitView->GetDirection();

}

void
TestWindow::MessageReceived(BMessage *message)
{
	switch (message->what)
	{
		case 'pres': {
			printf("TestWindow - ButtonPressed!!!\n");
			if (splitView->GetDirection() == B_HORIZONTAL) {
				printf("TestWindow - Setze SplitView auf B_VERTICAL!!!\n");
				splitView->SetDirection(B_VERTICAL);
			} else {
				printf("TestWindow - Setze SplitView auf B_HORIZONTAL!!!\n");
				splitView->SetDirection(B_HORIZONTAL);
			}
			
			break;
		}

		default:
		{
			BWindow::MessageReceived(message);
			break;
		}	
	}
}

bool
TestWindow::QuitRequested(void)
{
	printf("TestWindow -- QuitRequested\n");
	be_app->PostMessage(B_QUIT_REQUESTED);
	return false;
}
