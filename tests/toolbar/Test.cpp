// test.cpp

#include <stdio.h>

#include <Alert.h>
#include <Application.h>
#include <Bitmap.h>
#include <File.h>
#include <Resources.h>
#include <Window.h>

#include "Toolbar.h"
#include "ToolbarBitmapButton.h"
#include "ToolbarSeparator.h"

#define TEST_MESSAGE 'test'

class MyWindow : public BWindow { // --------------------------------------------------------------
private:
	WToolbar *fToolbar;
public:

MyWindow(void) :
	BWindow(BRect(100, 100, 400, 400), "Toolbar test", B_TITLED_WINDOW, B_ASYNCHRONOUS_CONTROLS)
{
	BView *back = new BView(Bounds(), NULL, B_FOLLOW_ALL_SIDES, 0);
	AddChild(back);
	fToolbar = new WToolbar(BRect(0, 0, 10, 10), "toolbar");
	fToolbar->SetPictureSize(W_TOOLBAR_PICTURE_MEDIUM);
	back->AddChild(fToolbar);
	fToolbar->AddItem(new WToolbarButton("button_0", "Hello!", new BMessage(TEST_MESSAGE)));
	fToolbar->AddItem(new WToolbarSeparator("separator_2"));
	LoadTrackerIcons();
	fToolbar->AddItem(new WToolbarSeparator("separator_1"));
	fToolbar->AddItem(new WToolbarButton("button_1", "Bye!", new BMessage(TEST_MESSAGE)));
	ResizeTo(fToolbar->Frame().Width(), fToolbar->Frame().Height());
}

void LoadTrackerIcons(void)
{
	const void *data_small, *data_large;
	size_t size_large, size_small;
	WToolbarBitmapButton *button;
	BBitmap *small, *large;
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

	index = 0;
	while (res.GetResourceInfo('ICON', index, &id, NULL, NULL)) {
		if (res.GetResourceInfo('MICN', id, NULL, NULL)) {
			data_small = res.LoadResource('MICN', id, &size_small);
			if (data_small != NULL) {
				data_large = res.LoadResource('ICON', id, &size_large);
				if (data_large != NULL) {
					small = new BBitmap(BRect(0, 0, 15, 15), B_CMAP8);
					small->SetBits(data_small, size_small, 0, B_CMAP8);
					large = new BBitmap(BRect(0, 0, 31, 31), B_CMAP8);
					large->SetBits(data_large, size_large, 0, B_CMAP8);

					sprintf(name, "bitmap_%d", index);
					button = new WToolbarBitmapButton(name, name, new BMessage(TEST_MESSAGE));
					button->AddBitmapSet(small);
					button->AddBitmapSet(large);
					fToolbar->AddItem(button, index / 10);
				}
			}
		}
		index++;
	}
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

MyApp(void) : BApplication("application/toolbar-test")
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

