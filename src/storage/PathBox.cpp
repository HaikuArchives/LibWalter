/*
	PathBox.h: A control to easily allow the user to choose a file/folder path
	Written by DarkWyrm <darkwyrm@earthlink.net>, Copyright 2007
	Released under the MIT license.
*/
#include "PathBox.h"
#include <Messenger.h>
#include <Window.h>

enum {
	M_PATHBOX_CHANGED = 'pbch',
	M_SHOW_FILEPANEL
};

PathBox::PathBox(const BRect &frame, const char *name, const char *path,
				const char *label, const int32 &resize, const int32 &flags)
 :	BView(frame,name,resize,flags)
{
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));

	BMessenger msgr(this);
	BEntry entry(path);
	entry_ref ref;
	entry.GetRef(&ref);
	
	fFilePanel = new BFilePanel(B_OPEN_PANEL, &msgr, &ref, 0, false);
	
	fBrowseButton = new BButton(BRect(0,0,1,1),"browse","Browse…",
								new BMessage(M_SHOW_FILEPANEL),
								B_FOLLOW_RIGHT | B_FOLLOW_TOP);
	fBrowseButton->ResizeToPreferred();
	fBrowseButton->MoveTo( frame.right - fBrowseButton->Bounds().Width() - 10, 10);
	AddChild(fBrowseButton);
	
	
	
	fPathControl = new BTextControl(BRect(10,10,11,11),"path",label,path,
									new BMessage(M_PATHBOX_CHANGED),
									B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	
	float w,h;
	
	if (B_BEOS_VERSION <= B_BEOS_VERSION_5) {
		BWindow *win = new BWindow(BRect(100,100,400,400),"",B_TITLED_WINDOW,0);
		win->AddChild(fPathControl);
		fPathControl->GetPreferredSize(&w,&h);
		fPathControl->RemoveSelf();
		win->Quit();
	} else {
		fPathControl->GetPreferredSize(&w,&h);
	}
	fPathControl->ResizeTo(fBrowseButton->Frame().left - 20, h);
	AddChild(fPathControl);
}


PathBox::PathBox(BMessage *data)
 :	BView(data)
{
}


PathBox::~PathBox(void)
{
}


BArchivable *
PathBox::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PathBox"))
		return new PathBox(data);

	return NULL;
}


status_t
PathBox::Archive(BMessage *data, bool deep) const
{
}


void
PathBox::ResizeToPreferred(void)
{
	float w,h;
	GetPreferredSize(&w,&h);
	ResizeTo(w,h);
	fPathControl->ResizeToPreferred();
	fBrowseButton->ResizeToPreferred();
	fBrowseButton->MoveTo(fPathControl->Frame().right + 10, 10);
}


void
PathBox::GetPreferredSize(float *w, float *h)
{
	float width, height;
	float tempw, temph;
	
	if (Window()) {
		fPathControl->GetPreferredSize(&width, &height);
	} else {
		if (B_BEOS_VERSION <= B_BEOS_VERSION_5) {
			fPathControl->RemoveSelf();
			BWindow *win = new BWindow(BRect(100,100,400,400),"",B_TITLED_WINDOW,0);
			win->AddChild(fPathControl);
			fPathControl->GetPreferredSize(&width,&height);
			fPathControl->RemoveSelf();
			win->Quit();
			AddChild(fPathControl);
		} else {
			fPathControl->GetPreferredSize(&width, &height);
		}
	}
	
	fBrowseButton->GetPreferredSize(&tempw, &temph);
	width += tempw + 30;
	height = (height > temph) ? height : temph;
	
	if (w)
		*w = width;
	if (h)
		*h = height + 20;
}

	
status_t
PathBox::GetSupportedSuites(BMessage *msg)
{
}


BHandler *
PathBox::ResolveSpecifier(BMessage *msg, int32 index,
							BMessage *specifier, int32 form,
							const char *property)
{
}


void
PathBox::MessageReceived(BMessage *msg)
{
}


void
PathBox::SetPath(const char *path)
{
}


void
PathBox::SetPath(const entry_ref &ref)
{
}


const char *
PathBox::Path(void) const
{
	return fPathControl->Text();
}


BFilePanel *
PathBox::FilePanel(void) const
{
	return fFilePanel;
}


void
PathBox::_InitObject(void)
{
	
}

