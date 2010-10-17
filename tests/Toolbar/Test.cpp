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

//#define DEBUG_ARCHIVE 1

#define MSG_TEST			'test'
#define MSG_ARCHIVE			'arch'
#define MSG_LABELS_NO		'nola'
#define MSG_LABELS_BOTTOM	'bott'
#define MSG_LABELS_SIDE		'side'
#define MSG_PICS_NO			'nopi'
#define MSG_PICS_SMALL		'smal'
#define MSG_PICS_LARGE		'larg'
#define MSG_ENABLE			'enab'
#define MSG_HORIZONTAL		'hori'
#define MSG_VERTICAL		'vert'
#define MSG_AUTOSIZE		'auto'
#define MSG_RESIZE			'resi'

class MyWindow : public BWindow { // --------------------------------------------------------------
private:
	WToolbar *fToolbar;
	BView *fBack;
	BMenu *fMenu;
public:

MyWindow(void) :
	BWindow(BRect(100, 100, 400, 400), "Toolbar test", B_TITLED_WINDOW, B_ASYNCHRONOUS_CONTROLS)
{
	BMenuBar *bar;
	BMenu *menu;
	BRect r;

	bar = new BMenuBar(Bounds(), "menubar");
	menu = new BMenu("Actions");
	menu->AddItem(new BMenuItem("Archive", new BMessage(MSG_ARCHIVE)));
	bar->AddItem(menu);
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
	fMenu->AddSeparatorItem();
	fMenu->AddItem(new BMenuItem("Horizontal", new BMessage(MSG_HORIZONTAL)));
	fMenu->AddItem(new BMenuItem("Vertical", new BMessage(MSG_VERTICAL)));
	fMenu->AddSeparatorItem();
	fMenu->AddItem(new BMenuItem("Auto size", new BMessage(MSG_AUTOSIZE)));
	fMenu->AddItem(new BMenuItem("Resize to preferred", new BMessage(MSG_RESIZE)));
	fMenu->ItemAt(0)->SetMarked(true);
	fMenu->ItemAt(6)->SetMarked(true);
	fMenu->ItemAt(8)->SetMarked(true);
	fMenu->ItemAt(10)->SetMarked(true);
	fMenu->ItemAt(13)->SetMarked(true);
	AddChild(bar);

	r = Bounds();
	r.top = bar->Frame().bottom + 1;
	fBack = new BView(r, NULL, B_FOLLOW_ALL_SIDES, 0);
	AddChild(fBack);

#ifdef DEBUG_ARCHIVE
	BFile file("/boot/home/ToolbarArchive", B_READ_ONLY);
	BMessage archive;
	archive.Unflatten(&file);
	BArchivable *archivable = WToolbar::Instantiate(&archive);
	fToolbar = static_cast<WToolbar*>(archivable);
#else
	fToolbar = new WToolbar(BRect(0, 0, 10, 10), "toolbar", W_TOOLBAR_STYLE_FLAT);
	fToolbar->SetPictureSize(W_TOOLBAR_PICTURE_MEDIUM);
	LoadTrackerIcons();
#endif

	fBack->AddChild(fToolbar);

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
	if (file.SetTo("/boot/system/lib/libtracker.so", B_READ_ONLY) != B_OK)
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
					sprintf(name, "bitmap_%d", (int)index);
					button = new WToolbarButton(name, name, NULL, new BMessage(MSG_TEST));
					button->SetPicture(small);
					button->SetPicture(large);
					if ((index - 5) % 10 == 0)
						fToolbar->AddItem(new WToolbarSeparator(), index / 10);
					fToolbar->AddItem(button, (index % 10 == 0 ? -1 : index / 10));
					if (index % 10 == 9)
						button->SetFlexible(true);
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
		case MSG_ARCHIVE: {
			BMessage archive;
			BFile file("/boot/home/ToolbarArchive", B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
			if (file.InitCheck() == B_OK) {
				fToolbar->Archive(&archive);
				if (archive.Flatten(&file) == B_OK)
					(new BAlert("Archive", "Toolbar archived in /boot/home/ToolbarArchive", "OK"))->Go();
			} else
				(new BAlert("Archive", "Error creating /boot/home/ToolbarArchive", "OK"))->Go();
			} break;
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
		case MSG_HORIZONTAL:
			fToolbar->SetAlignment(W_TOOLBAR_HORIZONTAL);
			fMenu->ItemAt(10)->SetMarked(true);
			fMenu->ItemAt(11)->SetMarked(false);
			if (!fToolbar->AutoSize()) {
				float height;
				fToolbar->GetPreferredSize(NULL, &height);
				fToolbar->SetResizingMode(B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
				fToolbar->ResizeTo(fBack->Frame().Width(), height);
			}
			break;
		case MSG_VERTICAL:
			fToolbar->SetAlignment(W_TOOLBAR_VERTICAL);
			fMenu->ItemAt(10)->SetMarked(false);
			fMenu->ItemAt(11)->SetMarked(true);
			if (!fToolbar->AutoSize()) {
				float width;
				fToolbar->GetPreferredSize(&width, NULL);
				fToolbar->SetResizingMode(B_FOLLOW_LEFT | B_FOLLOW_TOP_BOTTOM);
				fToolbar->ResizeTo(width, fBack->Frame().Height());
			}
			break;
		case MSG_AUTOSIZE:
			if (fMenu->ItemAt(13)->IsMarked()) {
				float width, height;
				fToolbar->SetAutoSize(false);
				fMenu->ItemAt(13)->SetMarked(false);
				if (fToolbar->Alignment() == W_TOOLBAR_HORIZONTAL) {
					width = fBack->Frame().Width();
					fToolbar->GetPreferredSize(NULL, &height);
					fToolbar->SetResizingMode(B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
				}
				else {
					fToolbar->GetPreferredSize(&width, NULL);
					height = fBack->Frame().Height();
					fToolbar->SetResizingMode(B_FOLLOW_LEFT | B_FOLLOW_TOP_BOTTOM);
				}
				fToolbar->ResizeTo(width, height);
			}
			else {
				fToolbar->SetResizingMode(B_FOLLOW_LEFT | B_FOLLOW_TOP);
				fToolbar->SetAutoSize(true);
				fMenu->ItemAt(13)->SetMarked(true);
			}
			break;
		case MSG_RESIZE:
			if (!fToolbar->AutoSize()) {
				float width, height;
				fToolbar->GetPreferredSize(&width, &height);
				if (fToolbar->Alignment() == W_TOOLBAR_HORIZONTAL)
					width = fBack->Frame().Width();
				else
					height = fBack->Frame().Height();
				fToolbar->ResizeTo(width, height);
			}
			break;
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

