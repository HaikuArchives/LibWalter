#include "MainView.h"
#include <TranslationUtils.h>
#include <stdio.h>
#include <Message.h>
#include <Window.h>
#include <ListView.h>
#include <List.h>
#include <ScrollView.h>
#include <Button.h>

#ifndef _APPLICATION_H
#include <Application.h>
#endif


MainView::MainView(BRect rect, const char *name) : BView (rect, name, B_FOLLOW_ALL, B_WILL_DRAW)
{

	myButton = new BButton(BRect(10,10,200,30),"aButton", "press me", new BMessage('pres'));
	AddChild(myButton);

}



void
MainView::MessageReceived(BMessage *msg) {

	switch (msg->what)
	{
		case 'pres': {
			printf("MainView - ButtonPressed!!!\n");
			break;
		}

		default:
		{
			BView::MessageReceived(msg);
			break;
		}	
	}
}


void MainView::AttachedToWindow() {

		myButton->SetTarget(Window());
		SetViewColor(230,230,230);
		
}
/*
void MainView::MouseDown(BPoint point) {

	Window()->SetTitle("Bottom clicked");

}

void MainView::SetTopView(BView *topView) {
	this->topView = topView;

}
*/
