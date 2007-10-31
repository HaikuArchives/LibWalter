// test.cpp

#include <stdio.h>

#include <Alert.h>
#include <Application.h>
#include <Bitmap.h>
#include <File.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <Resources.h>
#include <Window.h>

#include "Toolbar.h"
#include "ToolbarButton.h"
#include "ToolbarSeparator.h"

#define MSG_TEST			'test'
#define MSG_LABELS_NO		'nola'
#define MSG_LABELS_BOTTOM	'bott'
#define MSG_LABELS_SIDE		'side'
#define MSG_PICS_NO			'nopi'
#define MSG_PICS_SMALL		'smal'
#define MSG_PICS_LARGE		'larg'
#define MSG_ENABLE			'enab'

class MyWindow : public BWindow { // --------------------------------------------------------------
private:
	WToolbar *fToolbar;
	BMenu *fMenu;
public:

MyWindow(void) :
	BWindow(BRect(100, 100, 400, 400), "Toolbar test", B_TITLED_WINDOW, B_ASYNCHRONOUS_CONTROLS)
{
	BMenuBar *bar;
	BView *back;
	BRect r;

	bar = new BMenuBar(Bounds(), "menubar");
	fMenu = new BMenu("Customize");
	fMenu->AddItem(new BMenuItem("No labels", new BMessage(MSG_LABELS_NO)));
	fMenu->AddItem(new BMenuItem("Labels on bottom", new BMessage(MSG_LABELS_BOTTOM)));
	fMenu->AddItem(new BMenuItem("Labels on side", new BMessage(MSG_LABELS_SIDE)));
	fMenu->AddSeparatorItem();
	fMenu->AddItem(new BMenuItem("No pictures", new BMessage(MSG_PICS_NO)));
	fMenu->AddItem(new BMenuItem("Small pictures", new BMessage(MSG_PICS_SMALL)));
	fMenu->AddItem(new BMenuItem("Large pictures", new BMessage(MSG_PICS_LARGE)));
	bar->AddItem(fMenu);
	fMenu->AddSeparatorItem();
	fMenu->AddItem(new BMenuItem("Enabled", new BMessage(MSG_ENABLE)));
	fMenu->ItemAt(0)->SetMarked(true);
	fMenu->ItemAt(6)->SetMarked(true);
	fMenu->ItemAt(8)->SetMarked(true);
	AddChild(bar);

	r = Bounds();
	r.top = bar->Frame().bottom + 1;
	back = new BView(r, NULL, B_FOLLOW_ALL_SIDES, 0);
	AddChild(back);

	fToolbar = new WToolbar(BRect(0, 0, 10, 10), "toolbar");
	fToolbar->SetPictureSize(W_TOOLBAR_PICTURE_MEDIUM);
	back->AddChild(fToolbar);
	fToolbar->AddItem(new WToolbarButton("hello_button", "Hello!", NULL, new BMessage(MSG_TEST)));
	fToolbar->AddItem(new WToolbarSeparator("separator_1"));
	LoadTrackerIcons();
	fToolbar->AddItem(new WToolbarSeparator("separator_2"));
	fToolbar->AddItem(new WToolbarButton("bye_button", "Bye!", NULL, new BMessage(MSG_TEST)));

	ResizeTo(fToolbar->Frame().Width(), fToolbar->Frame().Height() + bar->Frame().Height() + 1);
}

void LoadTrackerIcons(void)
{
	const void *data_small, *data_large;
	size_t size_large, size_small;
	BBitmap *small[8], *large[8];
	WToolbarButton *button;
	int32 index, id;
	BResources res;
	char name[64];
	BFile file;

	// Open libtracker.so as a resources file
	if (file.SetTo("/boot/beos/system/lib/libtracker.so", B_READ_ONLY) != B_OK)
		return;
	if (res.SetTo(&file, false) != B_OK)
		return;

	// Each resource has a type and an ID. Each icon is stored in two resources,
	// one of type ICON (the large icon, 32x32) and the second of type MICN
	// (the small icon, 16x16), both with the same ID.
	// To get the couple, we first look for a resource of type ICON, store its
	// ID, and then for a resource of type MICN with that ID.
	// Icons are BBitmaps data, in B_CMAP8 color space.

	for (index = 0; index < 8; index++) {
		small[index] = NULL;
		large[index] = NULL;
	}
	index = 0;
	while (res.GetResourceInfo('ICON', index, &id, NULL, NULL)) {
		if (res.GetResourceInfo('MICN', id, NULL, NULL)) {
			data_small = res.LoadResource('MICN', id, &size_small);
			if (data_small != NULL) {
				data_large = res.LoadResource('ICON', id, &size_large);
				if (data_large != NULL) {
					small[0] = new BBitmap(BRect(0, 0, 15, 15), B_CMAP8);
					small[0]->SetBits(data_small, size_small, 0, B_CMAP8);
					large[0] = new BBitmap(BRect(0, 0, 31, 31), B_CMAP8);
					large[0]->SetBits(data_large, size_large, 0, B_CMAP8);
					sprintf(name, "bitmap_%d", index);
					button = new WToolbarButton(name, name, NULL, new BMessage(MSG_TEST));
					button->SetPicture(small);
					button->SetPicture(large);
					fToolbar->AddItem(button, index / 10);
				}
			}
		}
		index++;
	}
}

void MessageReceived(BMessage *message)
{
	switch (message->what) {
		case MSG_TEST:
			(new BAlert("Hello!", "Hello!", "OK"))->Go();
			break;
		case MSG_LABELS_NO:
			fToolbar->SetLabelPosition(W_TOOLBAR_LABEL_NONE);
			fMenu->ItemAt(0)->SetMarked(true);
			fMenu->ItemAt(1)->SetMarked(false);
			fMenu->ItemAt(2)->SetMarked(false);
			break;
		case MSG_LABELS_BOTTOM:
			fToolbar->SetLabelPosition(W_TOOLBAR_LABEL_BOTTOM);
			fMenu->ItemAt(0)->SetMarked(false);
			fMenu->ItemAt(1)->SetMarked(true);
			fMenu->ItemAt(2)->SetMarked(false);
			break;
		case MSG_LABELS_SIDE:
			fToolbar->SetLabelPosition(W_TOOLBAR_LABEL_SIDE);
			fMenu->ItemAt(0)->SetMarked(false);
			fMenu->ItemAt(1)->SetMarked(false);
			fMenu->ItemAt(2)->SetMarked(true);
			break;
		case MSG_PICS_NO:
			fToolbar->SetPictureSize(0);
			fMenu->ItemAt(4)->SetMarked(true);
			fMenu->ItemAt(5)->SetMarked(false);
			fMenu->ItemAt(6)->SetMarked(false);
			break;
		case MSG_PICS_SMALL:
			fToolbar->SetPictureSize(16);
			fMenu->ItemAt(4)->SetMarked(false);
			fMenu->ItemAt(5)->SetMarked(true);
			fMenu->ItemAt(6)->SetMarked(false);
			break;
		case MSG_PICS_LARGE:
			fToolbar->SetPictureSize(32);
			fMenu->ItemAt(4)->SetMarked(false);
			fMenu->ItemAt(5)->SetMarked(false);
			fMenu->ItemAt(6)->SetMarked(true);
			break;
		case MSG_ENABLE: {
			bool enabled = fMenu->ItemAt(8)->IsMarked();
			fToolbar->SetEnabled(!enabled);
			fMenu->ItemAt(8)->SetMarked(!enabled);
			} break;
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
	MyWindow *fWindow;
public:

MyApp(void) : BApplication("application/libwalter-Toolbar-test")
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

