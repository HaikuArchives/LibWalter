// test.cpp

#include <stdio.h>

#include <Application.h>
#include <Bitmap.h>
#include <File.h>
#include <Resources.h>
#include <Window.h>

#include "Toolbar.h"
#include "ToolbarBitmapButton.h"
#include "ToolbarSeparator.h"

class MyWindow : public BWindow { // --------------------------------------------------------------
private:
	Toolbar *fToolbar;
public:

MyWindow(void) :
	BWindow(BRect(100, 100, 400, 400), "Toolbar test", B_TITLED_WINDOW, B_ASYNCHRONOUS_CONTROLS)
{
	BView *back = new BView(Bounds(), NULL, B_FOLLOW_ALL_SIDES, 0);
	AddChild(back);
	fToolbar = new Toolbar(BRect(0, 0, 10, 10), "toolbar");
	fToolbar->SetPictureSize(TOOLBAR_PICTURE_MEDIUM);
	back->AddChild(fToolbar);
	fToolbar->AddControl(new ToolbarButton("button_0", "Hello!"));
	fToolbar->AddControl(new ToolbarSeparator("separator_2"));
	LoadTrackerIcons();
	fToolbar->AddControl(new ToolbarSeparator("separator_1"));
	fToolbar->AddControl(new ToolbarButton("button_1", "Bye!"));
	ResizeTo(fToolbar->Frame().Width(), fToolbar->Frame().Height());
}

void LoadTrackerIcons(void)
{
	const void *data_small, *data_large;
	size_t size_large, size_small;
	ToolbarBitmapButton *button;
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
					button = new ToolbarBitmapButton(name, name);
					button->AddBitmapSet(small);
					button->AddBitmapSet(large);
					fToolbar->AddControl(button, index / 10);
				}
			}
		}
		index++;
	}
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

MyApp(void) : BApplication("application/libwalter-toolbar-test")
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

