// Cursor and tooltip test

#include <Alert.h>
#include <Application.h>
#include <Cursor.h>
#include <List.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <View.h>
#include <Window.h>

#include "Cursor.h"

const unsigned char kCustomCursor[68] = {
	0x10, 0x01, 0x09, 0x07,
	0x00, 0x00, 0x00, 0x00, 0x70, 0x0E, 0x88, 0x11,
	0x88, 0x11, 0x87, 0xE1, 0x84, 0x21, 0x40, 0x02,
	0x26, 0x64, 0x12, 0x48, 0x20, 0x04, 0x20, 0x04,
	0x24, 0x24, 0x23, 0xC4, 0x10, 0x08, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x70, 0x0E, 0xF8, 0x1F,
	0xF8, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xFE,
	0x3F, 0xFC, 0x1F, 0xF8, 0x3F, 0xFC, 0x3F, 0xFC,
	0x3F, 0xFC, 0x3F, 0xFC, 0x1F, 0xF8, 0x00, 0x00
};

// Application messages
#define MSG_CREATE_DOCUMENT_WINDOW	'newW'
#define MSG_DOCUMENT_WINDOW_CLOSED	'Wclo'

// Declarations ---------------------------------------------------------------

class MyWindow : public BWindow {
private:
	BView *		fRed;
	BView *		fGreen;
	BView *		fBlue;
	BView *		fYellow;
	BMenu *		fRedMenu;
	BMenu *		fGreenMenu;
	BMenu *		fBlueMenu;
	BMenu *		fYellowMenu;
	BMenu *		fAppMenu;
	BMenuItem *	fAppImpose;
	BMenu *		fWindowMenu;
	BMenuItem *	fWindowImpose;
public:
				MyWindow(BPoint location);
	void		MessageReceived(BMessage *message);
	bool		QuitRequested(void);
};

class MyApp : public BApplication {
private:
	float	fLastWindowLocation;
	int		fDocumentWindows;
public:	
			MyApp(void);
	void	MessageReceived(BMessage *message);
	void	ReadyToRun(void);
};

// MyWindow -------------------------------------------------------------------

BView * _add_colored_view(BView *parent, BRect frame, const char *name,
	uint8 red, uint8 green, uint8 blue)
{
	BView *view = new BView(frame, name, B_FOLLOW_LEFT | B_FOLLOW_RIGHT, 0);
	view->SetViewColor(red, green, blue);
	parent->AddChild(view);
	SetViewCursor(view, B_CURSOR_I_BEAM);
	return view;
}

BMenu * _build_cursor_kind_menu(const char *label, int32 what,
	int32 item_to_mark)
{
	BMenu *menu = new BMenu(label);
	menu->AddItem(new BMenuItem("No special cursor", new BMessage(what)));
	menu->AddItem(new BMenuItem("Custom", new BMessage(what)));
	menu->AddItem(new BMenuItem("Hand", new BMessage(what)));
	menu->AddItem(new BMenuItem("I-beam", new BMessage(what)));
	/*#ifdef _ZETA_
	menu->AddItem(new BMenuItem("Link", new BMessage(what)));
	menu->AddItem(new BMenuItem("Horizontal splitter", new BMessage(what)));
	menu->AddItem(new BMenuItem("Vertical splitter", new BMessage(what)));
	#endif*/
	menu->ItemAt(item_to_mark)->SetMarked(true);
	menu->SetRadioMode(true);
	return menu;
}

BMenu * _build_complete_cursor_menu(BMenu **menu, BMenuItem **impose,
	const char *label, int32 what, int32 item_to_mark, int32 what_impose,
	bool mark_impose)
{
	BMenu *ret = new BMenu(label);
	*menu = _build_cursor_kind_menu("Kind", what, item_to_mark);
	ret->AddItem(*menu);
	ret->AddSeparatorItem();
	*impose = new BMenuItem("Impose", new BMessage(what_impose));
	if (mark_impose) (*impose)->SetMarked(true);
	ret->AddItem(*impose);
	return ret;
}

void _set_view_cursor(BView *view, BMenu *menu)
{
	switch (menu->IndexOf(menu->FindMarked())) {
		case 0:	// No special cursor
			UnsetViewCursor(view);
			break;
		case 1:	// Custom cursor
			SetViewCursor(view, kCustomCursor);
			break;
		case 2:	// Hand cursor
			SetViewCursor(view, B_CURSOR_SYSTEM_DEFAULT);
			break;
		case 3:	// I-beam cursor
			SetViewCursor(view, B_CURSOR_I_BEAM);
			break;
		/*#ifdef _ZETA_
		case 4:	// Link cursor
			SetViewCursor(view, B_CURSOR_LINK);
			break;
		case 5:	// Horizontal splitter cursor
			SetViewCursor(view, B_CURSOR_H_SPLITTER);
			break;
		case 6:	// Vertical splitter cursor
			SetViewCursor(view, B_CURSOR_V_SPLITTER);
			break;
		#endif*/
		default: break;
	}
}

MyWindow::MyWindow(BPoint location) :
	BWindow(BRect(0, 0, 350, 350), "Cursor test",
		B_DOCUMENT_WINDOW, B_ASYNCHRONOUS_CONTROLS)
{
	BMenuItem *item;
	BMenuBar *bar;
	BMenu *menu;
	BView *back;
	BRect r;

	bar = new BMenuBar(Bounds(), NULL);

	// File menu
	menu = new BMenu("File");
	item = new BMenuItem("New window",
		new BMessage(MSG_CREATE_DOCUMENT_WINDOW));
	item->SetTarget(be_app_messenger);
	menu->AddItem(item);
	menu->AddItem(new BMenuItem("Close", new BMessage(B_QUIT_REQUESTED)));
	menu->AddSeparatorItem();
	item = new BMenuItem("Quit", new BMessage(B_QUIT_REQUESTED));
	item->SetTarget(be_app_messenger);
	menu->AddItem(item);
	bar->AddItem(menu);

	// Cursor menu
	menu = new BMenu("Cursor");
	menu->AddItem(_build_complete_cursor_menu(&fAppMenu, &fAppImpose,
		"Application", 'Acur', 0, 'Aimp', false));
	menu->AddItem(_build_complete_cursor_menu(&fWindowMenu, &fWindowImpose,
		"This window", 'Wcur', 0, 'Wimp', false));
	menu->AddSeparatorItem();
	fRedMenu = _build_cursor_kind_menu("Red", 'Rcur', 3);
	menu->AddItem(fRedMenu);
	fGreenMenu = _build_cursor_kind_menu("Green", 'Gcur', 3);
	menu->AddItem(fGreenMenu);
	fBlueMenu = _build_cursor_kind_menu("Blue", 'Bcur', 3);
	menu->AddItem(fBlueMenu);
	fYellowMenu = _build_cursor_kind_menu("Yellow", 'Ycur', 3);
	menu->AddItem(fYellowMenu);
	bar->AddItem(menu);

	AddChild(bar);

	r = Bounds();
	r.top = bar->Frame().bottom + 1;
	back = new BView(r, NULL, B_FOLLOW_ALL_SIDES, 0);
	AddChild(back);

	// Color squares
	fRed = _add_colored_view(back, BRect(50, 50, 150, 150), "red", 255, 0, 0);
	fGreen = _add_colored_view(back, BRect(200, 50, 300, 150), "green", 0,
		255, 0);
	fBlue = _add_colored_view(back, BRect(50, 200, 150, 300), "blue", 0, 0,
		255);
	fYellow = _add_colored_view(back, BRect(200, 200, 300, 300), "yellow",
		255, 255, 0);

	ResizeBy(0, bar->Frame().Height());
	MoveTo(location);
}

void MyWindow::MessageReceived(BMessage *message)
{
	bool impose;
	switch (message->what) {
		case 'Acur':
		case 'Aimp': {
			impose = fAppImpose->IsMarked();
			if (message->what == 'Aimp') {
				impose = !impose;
				fAppImpose->SetMarked(impose);
			}
			switch (fAppMenu->IndexOf(fAppMenu->FindMarked())) {
				case 0:	// No special cursor
					UnsetAppCursor();
					break;
				case 1:	// Custom cursor
					SetAppCursor(kCustomCursor, impose);
					break;
				case 2:	// Hand cursor
					SetAppCursor(B_CURSOR_SYSTEM_DEFAULT, impose);
					break;
				case 3:	// I-beam cursor
					SetAppCursor(B_CURSOR_I_BEAM, impose);
					break;
				/*#ifdef _ZETA_
				case 4:	// Link cursor
					SetAppCursor(B_CURSOR_LINK, impose);
					break;
				case 5:	// Horizontal splitter cursor
					SetAppCursor(B_CURSOR_H_SPLITTER, impose);
					break;
				case 6:	// Vertical splitter cursor
					SetAppCursor(B_CURSOR_V_SPLITTER, impose);
					break;
				#endif*/
				default: break;
			}
			} break;
		case 'Wcur':
		case 'Wimp': {
			impose = fWindowImpose->IsMarked();
			if (message->what == 'Wimp') {
				impose = !impose;
				fWindowImpose->SetMarked(impose);
			}
			switch (fWindowMenu->IndexOf(fWindowMenu->FindMarked())) {
				case 0:	// No special cursor
					UnsetWindowCursor(this);
					break;
				case 1:	// Custom cursor
					SetWindowCursor(this, kCustomCursor, impose);
					break;
				case 2:	// Hand cursor
					SetWindowCursor(this, B_CURSOR_SYSTEM_DEFAULT, impose);
					break;
				case 3:	// I-beam cursor
					SetWindowCursor(this, B_CURSOR_I_BEAM, impose);
					break;
				/*#ifdef _ZETA_
				case 4:	// Link cursor
					SetWindowCursor(this, B_CURSOR_LINK, impose);
					break;
				case 5:	// Horizontal splitter cursor
					SetWindowCursor(this, B_CURSOR_H_SPLITTER, impose);
					break;
				case 6:	// Vertical splitter cursor
					SetWindowCursor(this, B_CURSOR_V_SPLITTER, impose);
					break;
				#endif*/
				default: break;
			}
			} break;
		case 'Rcur':
			_set_view_cursor(fRed, fRedMenu);
			break;
		case 'Gcur':
			_set_view_cursor(fGreen, fGreenMenu);
			break;
		case 'Bcur':
			_set_view_cursor(fBlue, fBlueMenu);
			break;
		case 'Ycur':
			_set_view_cursor(fYellow, fYellowMenu);
			break;
		default:
			BWindow::MessageReceived(message);
	}
}

bool MyWindow::QuitRequested(void)
{
	be_app->PostMessage(MSG_DOCUMENT_WINDOW_CLOSED);
	return BWindow::QuitRequested();
}

// MyApp ----------------------------------------------------------------------

MyApp::MyApp(void) :
	BApplication("application/libwalter-Cursor-test")
{
	fLastWindowLocation = 50;
}

void MyApp::MessageReceived(BMessage *message)
{
	switch (message->what) {
		case MSG_CREATE_DOCUMENT_WINDOW:
			if (fLastWindowLocation >= 500)
				fLastWindowLocation = 25;
			fLastWindowLocation += 25;
			(new MyWindow(BPoint(fLastWindowLocation,
				fLastWindowLocation)))->Show();
			fDocumentWindows++;
			break;
		case MSG_DOCUMENT_WINDOW_CLOSED:
			fDocumentWindows--;
			if (fDocumentWindows <= 0)
				be_app->PostMessage(B_QUIT_REQUESTED);
			break;
		default:
			BApplication::MessageReceived(message);
	}
}

void MyApp::ReadyToRun(void)
{
	(new MyWindow(BPoint(50, 50)))->Show();
	fDocumentWindows = 1;
	BApplication::ReadyToRun();
}

// main() ---------------------------------------------------------------------

int main(int, char **)
{
	(new MyApp())->Run();
}
