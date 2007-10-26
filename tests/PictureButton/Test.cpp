// ============================================================================
//
// libwalter tests/PictureButton/Test.cpp
//
// Distributed under the terms of the MIT License.
//
// Authors:
//		Gabriele Biffi - http://www.biffuz.it/
//
// ============================================================================

// Standard C++ headers
#include <vector>
using std::vector;

// Standard C headers
#include <math.h>
#include <stdio.h>
#include <string.h>

// Haiku headers
#include <Application.h>
#include <Bitmap.h>
#include <Box.h>
#include <CheckBox.h>
#include <File.h>
#include <FilePanel.h>
#include <Menu.h>
#include <MenuField.h>
#include <MenuItem.h>
#include <Resources.h>
#include <View.h>
#include <Window.h>

// libwalter headers
#include "BitmapUtils.h"
#include "PictureButton.h"

// ============================================================================
// Local constants and defines
// ============================================================================

#define MSG_BUTTON			'test'
#define MSG_DEFAULT			'defa'
#define MSG_ENABLED			'enab'
#define MSG_PICTURE 		'pict'
#define MSG_WIDTH_PLUS		'wid+'
#define MSG_WIDTH_MINUS		'wid-'
#define MSG_HEIGHT_PLUS		'hei+'
#define MSG_HEIGHT_MINUS	'hei-'
#define MSG_RESIZE			'resi'
#define MSG_VISIBLE			'visi'
#define MSG_POSITION		'posi'
#define MSG_ARCHIVE			'arch'
#define MSG_UNARCHIVE		'unar'

// ============================================================================
// Local classes
// ============================================================================

class MyWindow : public BWindow {
private:
	void				_center_button(void);
	void				_load_tracker_icons(void);
	vector<BBitmap*>	icons;
	unsigned			icon;
	unsigned			clicks;
	PictureButton *		button;
	BCheckBox *			checkDefault;
	BCheckBox *			checkEnabled;
	BView *				container;
	BFilePanel *		panelArchive;
	BFilePanel *		panelUnarchive;
	BMenu *				menuVisible;
	BMenu *				menuPosition;
public:
	MyWindow(void);
	~MyWindow();
	void MessageReceived(BMessage *message);
	bool QuitRequested(void);
};

class MyApp : public BApplication {
private:
	MyWindow *win;
public:
	MyApp(void);
	void ReadyToRun(void);
};

// ============================================================================
// MyWindow
// ============================================================================

MyWindow::MyWindow(void) :
	BWindow(BRect(0.0, 0.0, 249.0, 499.0), "libwalter PictureButton test",
		B_DOCUMENT_WINDOW, B_ASYNCHRONOUS_CONTROLS)
{
	const float kSpacing = 5.0, kTop = kSpacing + 2.0, kHeight = 20.0;

	BRect frame1, frame2;
	BMenuField *field;
	float left, width;
	BButton *button1;
	BView *last;
	BMenu *menu;
	BBox *box;

	clicks = 0;
	icon = 0;

	_load_tracker_icons();

	// Button container
	frame1 = BRect(0.0, 0.0, 249.0, 249.0);
	container = new BView(frame1, NULL, 0, 0);
	container->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	AddChild(container);

	// Button
	button = new PictureButton(BRect(0, 0, 0, 0), "button", "No clicks",
		(icons.size() > 0 ? icons[0] : NULL), BD_POSITION_ABOVE,
		new BMessage(MSG_BUTTON), B_FOLLOW_H_CENTER | B_FOLLOW_V_CENTER);
	container->AddChild(button);
	button->ResizeToPreferred();
	_center_button();

	// Editing controls
	frame2 = BRect(0.0, 0.0, 249.0, 0.0);
	box = new BBox(frame2, NULL);
	AddChild(box);

	left = kSpacing + 2.0;
	width = (frame2.Width() - kSpacing * 2.0);

	frame1 = BRect(0.0, 0.0, width, kHeight);
	frame1.OffsetBy(kTop, left);
	checkDefault = new BCheckBox(frame1, NULL, "Default",
		new BMessage(MSG_DEFAULT));
	last = checkDefault;
	box->AddChild(last);

	frame1.OffsetTo(left, last->Frame().bottom + 1.0 + kSpacing);
	checkEnabled = new BCheckBox(frame1, NULL, "Enabled",
		new BMessage(MSG_ENABLED));
	checkEnabled->SetValue(B_CONTROL_ON);
	last = checkEnabled;
	box->AddChild(last);

	frame1.OffsetTo(left, last->Frame().bottom + 1.0 + kSpacing);
	button1 = new BButton(frame1, NULL, "Change picture",
		new BMessage(MSG_PICTURE));
	if (icons.size() < 2)
		button1->SetEnabled(false);
	last = button1;
	box->AddChild(last);

	frame1.OffsetTo(left, last->Frame().bottom + 1.0 + kSpacing);
	frame1.right = frame1.left + floor(frame1.Width() - kSpacing) / 2.0;
	button1 = new BButton(frame1, NULL, "Width +",
		new BMessage(MSG_WIDTH_PLUS));
	box->AddChild(button1);
	frame1.OffsetTo(frame1.right + kSpacing + 1.0, frame1.top);
	button1 = new BButton(frame1, NULL, "Width -",
		new BMessage(MSG_WIDTH_MINUS));
	frame1 = BRect(0.0, 0.0, width, kHeight);
	last = button1;
	box->AddChild(last);

	frame1.OffsetTo(left, last->Frame().bottom + 1.0 + kSpacing);
	frame1.right = frame1.left + floor(frame1.Width() - kSpacing) / 2.0;
	button1 = new BButton(frame1, NULL, "Height +",
		new BMessage(MSG_HEIGHT_PLUS));
	box->AddChild(button1);
	frame1.OffsetTo(frame1.right + kSpacing + 1.0, frame1.top);
	button1 = new BButton(frame1, NULL, "Height -",
		new BMessage(MSG_HEIGHT_MINUS));
	frame1 = BRect(0.0, 0.0, width, kHeight);
	last = button1;
	box->AddChild(last);

	frame1.OffsetTo(left, last->Frame().bottom + 1.0 + kSpacing);
	button1 = new BButton(frame1, NULL, "Resize to preferred",
		new BMessage(MSG_RESIZE));
	if (icons.size() < 2)
		button1->SetEnabled(false);
	last = button1;
	box->AddChild(last);

	menu = new BMenu("Visible");
	menu->AddItem(new BMenuItem("Picture and label",
		new BMessage(MSG_VISIBLE)));
	menu->AddItem(new BMenuItem("Label only", new BMessage(MSG_VISIBLE)));
	menu->AddItem(new BMenuItem("Picture only", new BMessage(MSG_VISIBLE)));
	menu->SetRadioMode(true);
	menu->SetLabelFromMarked(true);
	menu->ItemAt(0)->SetMarked(true);
	frame1.OffsetTo(left, last->Frame().bottom + 1.0 + kSpacing);
	field = new BMenuField(frame1, NULL, "Visible", menu);
	menuVisible = menu;
	last = field;
	box->AddChild(last);

	menu = new BMenu("Picture position");
	menu->AddItem(new BMenuItem("Above", new BMessage(MSG_POSITION)));
	menu->AddItem(new BMenuItem("Below", new BMessage(MSG_POSITION)));
	menu->AddItem(new BMenuItem("Left", new BMessage(MSG_POSITION)));
	menu->AddItem(new BMenuItem("Right", new BMessage(MSG_POSITION)));
	menu->SetRadioMode(true);
	menu->SetLabelFromMarked(true);
	menu->ItemAt(0)->SetMarked(true);
	frame1.OffsetTo(left, last->Frame().bottom + 1.0 + kSpacing);
	field = new BMenuField(frame1, NULL, "Picture position", menu);
	menuPosition = menu;
	last = field;
	box->AddChild(last);

	frame1.OffsetTo(left, last->Frame().bottom + 1.0 + kSpacing);
	button1 = new BButton(frame1, NULL, "Archive button",
		new BMessage(MSG_ARCHIVE));
	last = button1;
	box->AddChild(last);

	frame1.OffsetTo(left, last->Frame().bottom + 1.0 + kSpacing);
	button1 = new BButton(frame1, NULL, "Unarchive button",
		new BMessage(MSG_UNARCHIVE));
	last = button1;
	box->AddChild(last);

	// Resize the container, the controls box and the window
	box->ResizeTo(box->Frame().Width(), last->Frame().bottom + kSpacing + 2.0);
	box->MoveTo(0.0, 250.0);
	ResizeTo(250.0, 250.0 + box->Frame().Height());
	container->SetResizingMode(B_FOLLOW_ALL_SIDES);
	box->SetResizingMode(B_FOLLOW_LEFT_RIGHT | B_FOLLOW_BOTTOM);
	MoveTo(50.0, 50.0);

	// Archive/unarchive file panels
	panelArchive = new BFilePanel(B_SAVE_PANEL, new BMessenger(this), NULL,
		B_FILE_NODE, false, NULL, NULL, true, true);
	panelUnarchive = new BFilePanel(B_OPEN_PANEL, new BMessenger(this), NULL,
		B_FILE_NODE, false, NULL, NULL, true, true);
}

MyWindow::~MyWindow()
{
	delete panelArchive;
	delete panelUnarchive;
}

void MyWindow::_center_button(void)
{
	BRect frame1 = container->Frame(), frame2;
	frame2 = button->Frame();
	button->MoveTo(floor((frame1.Width() - frame2.Width()) / 2.0),
		floor((frame1.Height() - frame2.Height()) / 2.0));
}

void MyWindow::_load_tracker_icons(void)
{
	const void *data_small, *data_large;
	size_t size_large, size_small;
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
					icons.push_back(small);
					icons.push_back(large);
				}
			}
		}
		index++;
	}
}

void MyWindow::MessageReceived(BMessage *message)
{
	switch (message->what) {
		case MSG_BUTTON: {
			char label[18];
			clicks++;
			if (clicks == 1)
				strcpy(label, "1 click");
			else
				sprintf(label, "%u clicks", clicks);
			button->SetLabel(label);
			} break;
		case MSG_DEFAULT:
			button->MakeDefault(checkDefault->Value() == B_CONTROL_ON);
			_center_button();
			break;
		case MSG_ENABLED:
			button->SetEnabled(checkEnabled->Value() == B_CONTROL_ON);
			break;
		case MSG_PICTURE: {
			unsigned num = icons.size();
			if (num > 1) {
				if (++icon == num) icon = 0;
				button->SetPicture(icons[icon]);
			}
			} break;
		case MSG_WIDTH_PLUS: {
			BRect frame = button->Frame();
			button->ResizeTo(frame.Width() + 5.0, frame.Height());
			_center_button();
			} break;
		case MSG_WIDTH_MINUS: {
			BRect frame = button->Frame();
			float width = frame.Width();
			width = (width <= 5.0 ? 0.0 : width - 5.0);
			button->ResizeTo(width, frame.Height());
			_center_button();
			} break;
		case MSG_HEIGHT_PLUS: {
			BRect frame = button->Frame();
			button->ResizeTo(frame.Width(), frame.Height() + 5.0);
			_center_button();
			} break;
		case MSG_HEIGHT_MINUS: {
			BRect frame = button->Frame();
			float height = frame.Height();
			height = (height <= 5.0 ? 0.0 : height - 5.0);
			button->ResizeTo(frame.Width(), height);
			_center_button();
			} break;
		case MSG_RESIZE:
			button->ResizeToPreferred();
			_center_button();
			break;
		case MSG_VISIBLE:
			button->SetVisible(static_cast<BDVisible>(
				menuVisible->IndexOf(menuVisible->FindMarked())));
			break;
		case MSG_POSITION:
			button->SetPosition(static_cast<BDPosition>(
				menuPosition->IndexOf(menuPosition->FindMarked())));
			break;
		case MSG_ARCHIVE:
			panelArchive->Show();
			break;
		case MSG_UNARCHIVE:
			panelUnarchive->Show();
			break;
		case B_SAVE_REQUESTED: {
			const char *name;
			entry_ref ref;
			if (message->FindRef("directory", &ref) == B_OK &&
			  message->FindString("name", &name) == B_OK) {
			  	BDirectory dir;
			  	BFile file;
			  	if (dir.SetTo(&ref) == B_OK && file.SetTo(&dir, name,
			  	  B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE) == B_OK) {
			  		BMessage archive;
			  		if (button->Archive(&archive) == B_OK)
				  		status_t s =archive.Flatten(&file);
			  	}
			}
			} break;
		case B_REFS_RECEIVED: {
			entry_ref ref;
			if (message->FindRef("refs", &ref) == B_OK) {
				BFile file;
				if (file.SetTo(&ref, B_READ_ONLY) == B_OK) {
					BMessage archive;
					if (archive.Unflatten(&file) == B_OK) {
						BArchivable *archivable;
						archivable = PictureButton::Instantiate(&archive);
						if (archivable != NULL) {
							PictureButton *newButton =
								dynamic_cast<PictureButton*>(archivable);
							if (newButton != NULL) {
								int32 index;
								button->RemoveSelf();
								delete button;
								button = newButton;
								container->AddChild(button);
								_center_button();
								index = static_cast<int32>(button->Visible());
								if (index >= menuVisible->CountItems())
									index = 0;
								menuVisible->ItemAt(index)->SetMarked(true);
								index = static_cast<int32>(button->Position());
								if (index >= menuPosition->CountItems())
									index = 0;
								menuPosition->ItemAt(index)->SetMarked(true);
								checkDefault->SetValue(button->IsDefault() ?
									B_CONTROL_ON : B_CONTROL_OFF);
								checkEnabled->SetValue(button->IsEnabled() ?
									B_CONTROL_ON : B_CONTROL_OFF);
							}
							else
								delete archivable;
						}
					}
				}
			}
			} break;
	}
	BWindow::MessageReceived(message);
}

bool MyWindow::QuitRequested(void)
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return BWindow::QuitRequested();
}

// ============================================================================
// MyApp
// ============================================================================

MyApp::MyApp() :
	BApplication("application/libwalter-PictureButton-test")
{
	win = new MyWindow();
}

void MyApp::ReadyToRun(void)
{
	BApplication::ReadyToRun();
	win->Show();
}

int main(int, char**)
{
	(new MyApp())->Run();
	return 0;
}
