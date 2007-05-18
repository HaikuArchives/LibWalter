// =============================================================================
//
// libwalter Toolbar.Ccpp
//
// WToolbar, a toolbar widget
//
// Revision: 20070518
// Page width 80, tab width 4, encoding UTF-8, line endings LF.
//
// Original author:
//     Gabriele Biffi - http://www.biffuz.it/
// Contributors:
//
// Released under the terms of the MIT license.
//
// =============================================================================

// Standard C++ headers
#include <typeinfo>

// Standard C headers
#include <string.h>

// BeOS headers
#include <Bitmap.h>
#include <Handler.h>
#include <Looper.h>
#include <Menu.h>
#include <PropertyInfo.h>
#include <Window.h>

// libwalter headers
#include "Toolbar.h"
#include "ToolbarControl.h"
#include "ToolbarSupport.h"

// =============================================================================
// WToolbar
// =============================================================================

/*static property_info kWToolbarPropList[] = {
	{ "enabled",
	  { B_GET_PROPERTY, B_SET_PROPERTY, 0 },
	  { B_DIRECT_SPECIFIER, 0 },
	  NULL
	},
	{}
};*/

// Constructors and destructor

WToolbar::WToolbar(BRect frame, const char *name, int style,
	WToolbarAlignment alignment, bool auto_size, border_style border,
	uint32 resizing_mode, uint32 flags) :
	BView(frame, name, resizing_mode,
		flags | B_FRAME_EVENTS | B_FULL_UPDATE_ON_RESIZE | B_WILL_DRAW)
{
	_init_object();

	SetAlignment(alignment);
	SetAutoSize(auto_size);
	SetBorder(border);
	SetStyle(style);

	fDisableUpdate = false;
	Update();
}

WToolbar::WToolbar(BMessage *archive) :
	BView(archive)
{
	WToolbarLabelPosition labelPosition;
	float margin, padding, picSize;
	WToolbarAlignment alignment;
	BArchivable *archivable;
	bool autoSize, enabled;
	border_style border;
	WToolbarItem *item;
	BMessage message;
	BBitmap *backPic;
	int32 index;
	int style;

	_init_object();

	// Properties

	if (archive->FindBool("WToolbar::autosize", &autoSize) == B_OK)
		SetAutoSize(autoSize);

	if (archive->FindBool("WToolbar::enabled", &enabled) == B_OK)
		SetEnabled(enabled);

	// Appearance

	fDisableStyling = true;
	if (archive->FindInt32("WToolbar::style", (int32*)(&style)) == B_OK)
		SetStyle(style);
	fDisableStyling = false;

	if (archive->FindInt32("WToolbar::alignment",
	  (int32*)(&alignment)) == B_OK)
		SetAlignment(alignment);

	if (archive->FindMessage("WToolbar::background_bitmap",
	  &message) == B_OK) {
		archivable = instantiate_object(&message);
		if (archivable != NULL) {
			backPic = dynamic_cast<BBitmap*>(archivable);
			if (backPic != NULL) {
				if (backPic->IsValid())
					SetBackgroundBitmap(backPic);
				else
					delete backPic;
			}
			else
				delete archivable;
		}
	}

	if (archive->FindInt32("WToolbar::border", (int32*)(&border)) == B_OK)
		SetBorder(border);

	if (archive->FindInt32("WToolbar::label_position",
	  (int32*)(&labelPosition)) == B_OK)
		SetLabelPosition(labelPosition);

	if (archive->FindFloat("WToolbar::margin", &margin) == B_OK)
		SetMargin(margin);

	if (archive->FindFloat("WToolbar::padding", &padding) == B_OK)
		SetPadding(padding);

	if (archive->FindFloat("WToolbar::picture_size", &picSize) == B_OK)
		SetPictureSize(picSize);

	// Items

	index = 0;
	while (archive->FindMessage("WToolbar::item", index,
	  &message) == B_OK) {
		archivable = instantiate_object(&message);
		if (archivable != NULL) {
			item = dynamic_cast<WToolbarItem*>(archivable);
			if (item != NULL) {
				if (!AddItem(item, item->fLine))
					delete item;
			}
			else
				delete archivable;
		}
		index++;
	}

	// Force the update
	fDisableUpdate = false;
	Update();
}

WToolbar::~WToolbar()
{
	fDisableUpdate = true;

	// Delete all the items
	WToolbarItem *item;
	while (fItems.size() > 0) {
		item = fItems[0];
		RemoveItem(item);
		delete item;
	}

	// Other cleanups
	if (fAppearance.fBackBitmap != NULL)
		delete fAppearance.fBackBitmap;
}

// Private

void WToolbar::_draw_item(unsigned index, BRect updateRect)
{
	// Beware, this function does not perform any sanity checking!!!
	// TODO set the clipping region and update rect
	BPoint origin;
	origin = Origin();
	SetOrigin(fItems[index]->Frame().LeftTop());
	PushState();
	fItems[index]->Draw(this, fItems[index]->Bounds());
	PopState();
	SetOrigin(origin);
	Sync();
}

void WToolbar::_init_object(void)
{
	// Internals
	fDisableUpdate = true;
	fDisableStyling = false;
	fMouseDownItem = NULL;
	fMouseOverItem = NULL;

	// Properties
	fAutoSize = false;
	fEnabled = true;

	// Appearance (flat style)
	fAppearance.fAlignment = W_TOOLBAR_HORIZONTAL;
	fAppearance.fBackBitmap = NULL;
	fAppearance.fBorder = B_FANCY_BORDER;
	fAppearance.fLabelPosition = W_TOOLBAR_LABEL_NONE;
	fAppearance.fMargin = 0.0;
	fAppearance.fPadding = 2.0;
	fAppearance.fPicSize = static_cast<float>(W_TOOLBAR_PICTURE_XSMALL);
	fAppearance.fStyle = W_TOOLBAR_STYLE_FLAT;

	// BView properties
	SetViewColor(B_TRANSPARENT_32_BIT);
}

// Protected

void WToolbar::AssertLocked(void)
{
	// Why BView::check_lock() is private and not protected???
	if (Looper() == NULL) return;
	if (!Looper()->IsLocked())
		debugger("looper must be locked");
}

float WToolbar::BorderSize(void)
{
	return 2.0;
}

void WToolbar::DrawBackground(BRect updateRect)
{
	if (fAppearance.fBackBitmap != NULL) {
		const float kWidth = fAppearance.fBackBitmap->Bounds().Width(),
			kHeight = fAppearance.fBackBitmap->Bounds().Height(),
			kBorder = (fAppearance.fBorder == B_FANCY_BORDER ? BorderSize() :
				(fAppearance.fBorder == B_PLAIN_BORDER ? 1.0 : 0.0));
		float x, y;
		for (y = floor((updateRect.top - kBorder) / kHeight) * kHeight +
		  kBorder; y < updateRect.bottom; y += kHeight) {
			for (x = floor((updateRect.left - kBorder) / kWidth) * kWidth +
			  kBorder; x <= updateRect.right; x += kWidth)
				DrawBitmap(fAppearance.fBackBitmap, BPoint(x, y));
		}
	}
	else
		FillRect(updateRect, B_SOLID_LOW);
}

void WToolbar::DrawBorder(BRect updateRect)
{
	WToolbarSupport::Draw3DBorder(this, Bounds());
}

// BArchivable hooks

status_t WToolbar::Archive(BMessage *archive, bool deep) const
{
	status_t status;

	// Archive ancestor
	status = BView::Archive(archive, false);

	// Archive properties

	if (status == B_OK)
		status = archive->AddBool("WToolbar::autosize", fAutoSize);

	if (status == B_OK)
		status = archive->AddBool("WToolbar::enabled", fEnabled);

	// Archive appearance

	if (status == B_OK)
		status = archive->AddInt32("WToolbar::alignment",
			static_cast<int32>(fAppearance.fAlignment));

	if (status == B_OK)
		status = archive->AddInt32("WToolbar::style", fAppearance.fStyle);

	if (status == B_OK) {
		if (fAppearance.fBackBitmap != NULL &&
		  fAppearance.fBackBitmap->IsValid()) {
			BMessage bmp;
			status = fAppearance.fBackBitmap->Archive(&bmp);
			if (status == B_OK)
				status = archive->AddMessage("WToolbar::background_bitmap",
					&bmp);
		}
	}

	if (status == B_OK)
		status = archive->AddInt32("WToolbar::border",
			static_cast<int32>(fAppearance.fBorder));

	if (status == B_OK)
		status = archive->AddInt32("WToolbar::label_position",
			static_cast<int32>(fAppearance.fLabelPosition));

	if (status == B_OK)
		status = archive->AddFloat("WToolbar::margin", fAppearance.fMargin);

	if (status == B_OK)
		status = archive->AddFloat("WToolbar::padding", fAppearance.fPadding);

	if (status == B_OK)
		status = archive->AddFloat("WToolbar::picture_size",
			fAppearance.fPicSize);

	// Archive items

	if (deep) {
		const unsigned kItems = fItems.size();
		unsigned i;
		for (i = 0; i < kItems; i++) {
			if (status == B_OK) {
				BMessage item;
				status = fItems[i]->Archive(&item, true);
				if (status == B_OK)
					status = archive->AddMessage("WToolbar::item", &item);
			}
		}
	}

	return status;
}

BArchivable * WToolbar::Instantiate(BMessage *archive)
{
	return (validate_instantiation(archive, "WToolbar") ?
		new WToolbar(archive) : NULL);
}

// BHandler hooks

/*status_t WToolbar::GetSupportedSuites(BMessage *message)
{
	BPropertyInfo pi(kWToolbarPropList);
	message->AddString("suites", "suite/vnd.AKToolkit.Toolbar");
	message->AddFlat("messages", &pi);
	return BView::GetSupportedSuites(message);
}*/

void WToolbar::MessageReceived(BMessage *message)
{
	BMessage *reply = NULL;
	bool handled = false;

	/*switch (message->what) {
		case B_GET_PROPERTY:
		case B_SET_PROPERTY: {
			const char *property;
			int32 cur;
			if (message->GetCurrentSpecifier(&cur, NULL, NULL,
			  &property) == B_OK) {
				// TODO handle all other properties
				if (strcmp(property, "enabled")) {
					if (message->what == B_GET_PROPERTY) {
						reply = new BMessage(B_REPLY);
						reply->AddBool("result", Enabled());
						handled = true;
					}
					else {
						bool enabled;
						if (message->FindBool("data", &enabled) == B_OK) {
							SetEnabled(enabled);
							reply = new BMessage(B_REPLY);
							reply->AddInt32("error", B_OK);
							handled = true;
						}
					}
				}
			}
			} break;
	}*/

	if (handled) {
		if (reply != NULL) {
			message->SendReply(reply);
			delete reply;
		}
	}
	else
		BView::MessageReceived(message);
}

/*BHandler * WToolbar::ResolveSpecifier(BMessage *message, int32 index,
	BMessage *specifier, int32 what, const char *property)
{
	BPropertyInfo pi(kWToolbarPropList);
	if (pi.FindMatch(message, index, specifier, what, property) >= 0)
		return this;
	return BView::ResolveSpecifier(message, index, specifier, what, property);
}*/

// BView hooks

void WToolbar::AttachedToWindow(void)
{
	if (!fTarget.IsValid()) {
		const unsigned kItems = fItems.size();
		WToolbarControl *control;
		unsigned i;
		SetTarget(Window());
		for (i = 0; i < kItems; i++) {
			control = dynamic_cast<WToolbarControl*>(fItems[i]);
			if (control != NULL && !(control->Messenger().IsValid()))
				control->SetTarget(fTarget);
		}
	}
}

void WToolbar::DetachedFromWindow(void)
{
	BView::DetachedFromWindow();
}

void WToolbar::Draw(BRect updateRect)
{
	// Background
	PushState();
	DrawBackground(updateRect);
	PopState();

	// Border
	switch (fAppearance.fBorder) {
		case B_FANCY_BORDER:
			PushState();
			DrawBorder(updateRect);
			PopState();
			break;
		case B_PLAIN_BORDER:
			StrokeRect(Bounds(), B_SOLID_HIGH);
		default:
			break;
	}		

	for (unsigned i = 0; i < fItems.size(); i++) {
		if (fItems[i]->fFrame.Intersects(updateRect) &&
		  fItems[i]->fVisible)
			_draw_item(i, updateRect);
	}
}

void WToolbar::GetPreferredSize(float *width, float *height)
{
	unsigned i, visible_items, visible_lines;
	float w = 0.0, h = 0.0, max;
	WToolbarItem *item;
	int line;

	// Count visible items
	visible_items = 0;
	if (fItems.size() > 0) {
		for (i = 0; i < fItems.size(); i++) {
			if (fItems[i]->fVisible)
				visible_items++;
		}
	}

	// Calculate width and height
	if (visible_items > 0) {
		line = 0;
		visible_lines = 1;
		max = 0.0;
		if (fAppearance.fAlignment == W_TOOLBAR_VERTICAL) {
			// Height is the height of the tallest line, line height is the sum
			// of the heights of its visible items
			// Width is the width of any of the item times the visible lines
			for (i = 0; i < fItems.size(); i++) {
				item = fItems[i];
				if (item->fVisible) {
					if (item->fLine != line) {
						visible_lines++;
						line = item->fLine;
						if (h > max)
							max = h;
						h = 0.0;
					}
					h += item->fFrame.Height() + 1.0 + fAppearance.fMargin;
					if (w == 0.0)
						w = fItems[i]->fFrame.Width() + 1.0;
				}
			}
			h = (h > max ? h : max) + fAppearance.fMargin;
			w = fAppearance.fMargin + (w + fAppearance.fMargin) *
				static_cast<float>(visible_lines);
		}
		else {
			// Width is the width of the widest line, line width is the sum
			// of the widths of its visible items
			// Height is the height of any of the item times the visible
			// lines
			for (i = 0; i < fItems.size(); i++) {
				item = fItems[i];
				if (item->fVisible) {
					if (item->fLine != line) {
						visible_lines++;
						line = item->fLine;
						if (w > max)
							max = w;
						w = 0.0;
					}
					w += item->fFrame.Width() + 1.0 + fAppearance.fMargin;
					if (h == 0.0)
						h = fItems[i]->fFrame.Height() + 1.0;
				}
			}
			w = (w > max ? w : max) + fAppearance.fMargin;
			h = fAppearance.fMargin + (h + fAppearance.fMargin) *
				static_cast<float>(visible_lines);
		}
	}

	// Border
	switch (fAppearance.fBorder) {
		case B_FANCY_BORDER:
			h += BorderSize() * 2.0;
			w += BorderSize() * 2.0;
			break;
		case B_PLAIN_BORDER:
			h += 2.0;
			w += 2.0;
			break;
		default:
			break;
	}

	// Padding
	h += fAppearance.fPadding * 2.0;
	w += fAppearance.fPadding * 2.0;			

	if (height != NULL) *height = h - 1.0;
	if (width != NULL) *width = w - 1.0;
}

void WToolbar::MouseDown(BPoint point)
{
	if (Looper() == NULL) return;

	SetMouseEventMask(B_POINTER_EVENTS, B_LOCK_WINDOW_FOCUS |
		B_SUSPEND_VIEW_FOCUS);

	int32 button = B_PRIMARY_MOUSE_BUTTON;
	BMessage *msg;

	// Find the mouse button
	msg = Looper()->CurrentMessage();
	if (msg != NULL)
		msg->FindInt32("buttons", &button);

	// Left button
	if (button == B_PRIMARY_MOUSE_BUTTON) {

		// If the mouse is over a item, tell the item about this
		if (fMouseOverItem != NULL && fEnabled) {
			fMouseOverItem->MouseDown(point - fMouseOverItem->fFrame.LeftTop());
			fMouseDownItem = fMouseOverItem;
		}
	}
}

void WToolbar::MouseMoved(BPoint point, uint32 transit,
	const BMessage *message)
{
	if (Looper() == NULL) return;

	if (fMouseDownItem == NULL) {

		int32 buttons = 0;
		BMessage *msg;

		// Pressed mouse buttons
		msg = Looper()->CurrentMessage();
		if (msg != NULL)
			msg->FindInt32("buttons", &buttons);

		if (buttons == 0) {
			if (transit == B_ENTERED_VIEW || transit == B_INSIDE_VIEW) {
				WToolbarItem *old_over = fMouseOverItem;
				fMouseOverItem = ItemAt(point);
				if (fMouseOverItem != old_over) {
					// Remove mouse over from previous item
					if (old_over != NULL)
						old_over->MouseMoved(point - old_over->fFrame.LeftTop(),
							B_EXITED_VIEW, message);
					// Assign mouse over to new item
					if (fMouseOverItem != NULL)
						fMouseOverItem->MouseMoved(
							point - fMouseOverItem->fFrame.LeftTop(),
							B_ENTERED_VIEW, message);
				}
				else {
					// Tell the item that the mouse has moved
					if (fMouseOverItem != NULL)
						fMouseOverItem->MouseMoved(
							point - fMouseOverItem->fFrame.LeftTop(),
							B_INSIDE_VIEW, message);
				}
			}
			else {
				// B_EXITED_VIEW or B_OUTSIDE_VIEW, remove focus
				if (fMouseOverItem != NULL) {
					fMouseOverItem->MouseMoved(
						point - fMouseOverItem->fFrame.LeftTop(),
						B_EXITED_VIEW, message);
					fMouseOverItem = NULL;
				}
			}
		}
	}

	// If the mouse is down, we need to remove the mouse over from the currently
	// pressed item, but not assign it to a new one.
	else {
		if (transit == B_ENTERED_VIEW || transit == B_INSIDE_VIEW) {
			WToolbarItem *new_over = ItemAt(point);
			if (new_over != fMouseDownItem) {
				// Remove mouse over from pressed item
				fMouseDownItem->MouseMoved(point -
					fMouseDownItem->fFrame.LeftTop(), B_EXITED_VIEW, message);
			}
			else {
				// Tell the item that the mouse has moved
				fMouseDownItem->MouseMoved(point -
					fMouseDownItem->fFrame.LeftTop(), B_INSIDE_VIEW, message);
			}
		}
		else {
			// B_EXITED_VIEW or B_OUTSIDE_VIEW, remove focus
			fMouseDownItem->MouseMoved(point - fMouseDownItem->fFrame.LeftTop(),
				B_EXITED_VIEW, message);
		}
	}
}

void WToolbar::MouseUp(BPoint point)
{
	if (Looper() == NULL) return;

	WToolbarItem *new_over;
	BMessage *msg;

	msg = Looper()->CurrentMessage();

	// If an item is pressed, tell it what is happening
	if (fMouseDownItem != NULL) {
		fMouseDownItem->MouseUp(point - fMouseDownItem->fFrame.LeftTop());
		fMouseDownItem = NULL;
	}

	// Check which item the mouse is over
	new_over = ItemAt(point);
	if (new_over != fMouseOverItem) {
		if (fMouseOverItem != NULL)
			fMouseOverItem->MouseMoved(point - fMouseOverItem->fFrame.LeftTop(),
				B_EXITED_VIEW, msg);
		if (new_over != NULL)
			new_over->MouseMoved(point - new_over->fFrame.LeftTop(),
				B_ENTERED_VIEW, msg);
		fMouseOverItem = new_over;
	}
	else {
		if (fMouseOverItem != NULL)
			fMouseOverItem->MouseMoved(point - fMouseOverItem->fFrame.LeftTop(),
				B_INSIDE_VIEW, msg);
	}
}

// Items management

bool WToolbar::AddItem(WToolbarItem *item, int line, int position)
{
	// Items are stored by line, then by position.

	AssertLocked();
	if (item->Toolbar() != NULL)
		return (item->Toolbar() == this);
	if (fItems.size() >= W_TOOLBAR_MAX_ITEMS)
		return false;

	bool disable_update = fDisableUpdate;
	fDisableUpdate = true;

	if (line >= 0 && line <= CountLines()) {
		// Insert the item at the given line and, if possible, position
		WToolbarItems::iterator i;
		unsigned _first = 0, _pos = 0;
		// Find the first item of the line
		while (_first < fItems.size()) {
			if (fItems[_first]->fLine == line) break;
			_first++;
		}
		// Find the item at position, if any
		while (_first + _pos < fItems.size()) {
			if ((int)_pos == position) break;
			if (fItems[_first + _pos]->fLine == line + 1) break;
			_pos++;
		}
		// Insert the item
		item->fLine = line;
		i = fItems.begin() + _first + _pos;
		fItems.insert(i, item);
	}
	else {
		// Insert the item at the end of the last line
		item->fLine = (CountLines() > 0 ? CountLines() - 1 : 0);
		fItems.push_back(item);
	}

	item->fToolbar = this;
	item->AttachedToToolbar();

	fDisableUpdate = disable_update;
	Update();
	return true;
}

int WToolbar::CountItems(void)
{
	return (int)(fItems.size());
}

int WToolbar::CountItemsInLine(int line)
{
	if (line < 0) return 0;
	int count = 0;
	for (unsigned i = 0; i < fItems.size(); i++) {
		if (fItems[i]->fLine == line) count++;
	}
	return count;
}

int WToolbar::CountLines(void)
{
	int lines = -1;
	for (unsigned i = 0; i < fItems.size(); i++) {
		if (fItems[i]->fLine > lines)
			lines = fItems[i]->fLine;
	}
	return lines + 1;
}

bool WToolbar::DrawItem(WToolbarItem *item)
{
	AssertLocked();
	if (item == NULL) return false;
	int index = IndexOf(item);
	if (index == -1) return false;
	if (Window() == NULL) return true;
	Invalidate(item->Frame());
	return true;
}

WToolbarItem * WToolbar::FindItem(const char *name)
{
	unsigned i = 0;
	while (i < fItems.size()) {
		if (name == NULL) {
			if (fItems[i]->Name() == NULL) return fItems[i];
		}
		else
			if (strcmp(name, fItems[i]->Name()) == 0) return fItems[i];
		i++;
	}
	return NULL;
}

int WToolbar::IndexOf(WToolbarItem *item)
{
	if (item == NULL) return -1;
	if (item->Toolbar() != this) return -1;
	unsigned i = 0;
	while (i < fItems.size()) {
		if (fItems[i] == item) return (int)i;
		i++;
	}
	return -1;
}

WToolbarItem * WToolbar::ItemAt(BPoint point)
{
	if (fItems.size() == 0) return NULL;
	unsigned i = 0;
	while (i < fItems.size()) {
		if (fItems[i]->fVisible && fItems[i]->fFrame.Contains(point))
			return fItems[i];
		i++;
	}
	return NULL;
}

WToolbarItem * WToolbar::ItemAt(int index)
{
	return (index >= 0 && index < (int)(fItems.size()) ?
		fItems[(unsigned)index] : NULL);
}

WToolbarItem * WToolbar::ItemAt(int line, int position, bool exact)
{
	if (fItems.size() == 0)
		return NULL;

	// If exact, exclude impossible values
	if (exact && (position < 0 || position >= (int)(fItems.size()) ||
	  line < 0 || line >= CountLines()))
		return NULL;

	unsigned _first = 0, _pos = 0;
	int _line;

	// What is the real line?
	if (line < 0)
		_line = 0;
	else if (line >= CountLines() - 1)
		_line = CountLines() - 1;
	else
		_line = line;

	// Find the first item of the line
	while (_first < fItems.size()) {
		if (fItems[_first]->fLine == _line) break;
		_first++;
	}

	// Look for the requested item
	if (position > 0) {
		while (true) {
			if (fItems[_first + _pos]->fLine != _line) {
				if (exact) return NULL;
				_pos--;
				break;
			}
			if ((int)_pos == position) break;
			if (_first + _pos == fItems.size() - 1) break;
			_pos++;
		}
	}

	return fItems[_first + _pos];
}

bool WToolbar::RemoveItem(WToolbarItem *item)
{
	if (item == NULL) return false;

	AssertLocked();

	bool disable_update;
	int index, line;

	// Index of the item
	index = IndexOf(item);
	if (index == -1) return false;

	// Make sure no updates are performed
	disable_update = fDisableUpdate;
	fDisableUpdate = true;

	// Remove the item
	if (fMouseDownItem == item)
		fMouseDownItem = NULL;
	line = item->fLine;
	item->fToolbar = NULL;
	item->fFrame = BRect(0.0, 0.0, -1.0, -1.0);
	item->fLine = -1;
	item->DetachedFromToolbar();
	fItems.erase(fItems.begin() + (unsigned)index);

	// Make sure we don't leave empty lines
	if (CountItemsInLine(line) == 0) {
		while (fItems.size() > (unsigned)index) {
			fItems[index]->fLine--;
			index++;
		}
	}

	fDisableUpdate = disable_update;
	Update();
	return true;
}

// Properties

bool WToolbar::AutoSize(void)
{
	return fAutoSize;
}

bool WToolbar::Enabled(void)
{
	return fEnabled;
}

void WToolbar::SetAutoSize(bool auto_size)
{
	AssertLocked();
	if (auto_size == fAutoSize) return;
	fAutoSize = auto_size;
	Update();
}

void WToolbar::SetEnabled(bool enabled)
{
	AssertLocked();
	if (enabled == fEnabled) return;
	fEnabled = enabled;
	Update();
	Invalidate();
}

// Appearance

WToolbarAlignment WToolbar::Alignment(void)
{
	return fAppearance.fAlignment;
}

BBitmap * WToolbar::BackgroundBitmap(void)
{
	return fAppearance.fBackBitmap;
}

border_style WToolbar::Border(void)
{
	return fAppearance.fBorder;
}

WToolbarLabelPosition WToolbar::LabelPosition(void)
{
	return fAppearance.fLabelPosition;
}

float WToolbar::Margin(void)
{
	return fAppearance.fMargin;
}

float WToolbar::Padding(void)
{
	return fAppearance.fPadding;
}

float WToolbar::PictureSize(void)
{
	return fAppearance.fPicSize;
}

void WToolbar::SetAlignment(WToolbarAlignment alignment)
{
	AssertLocked();
	if (alignment == fAppearance.fAlignment ||
	  (alignment != W_TOOLBAR_HORIZONTAL && alignment != W_TOOLBAR_VERTICAL))
		return;
	fAppearance.fAlignment = alignment;
	Update();
}

void WToolbar::SetBackgroundBitmap(BBitmap *bitmap)
{
	AssertLocked();
	if (bitmap == fAppearance.fBackBitmap) return;
	if (fAppearance.fBackBitmap != NULL) {
		delete fAppearance.fBackBitmap;
		fAppearance.fBackBitmap = NULL;
	}
	if (bitmap != NULL)
		if (bitmap->IsValid())
			fAppearance.fBackBitmap = bitmap;
	Update();
	Invalidate();
}

void WToolbar::SetBorder(border_style border)
{
	AssertLocked();
	if (border == fAppearance.fBorder || (border != B_FANCY_BORDER &&
	  border != B_PLAIN_BORDER && border != B_NO_BORDER))
		return;
	fAppearance.fBorder = border;
	Update();
}

void WToolbar::SetLabelPosition(WToolbarLabelPosition label_position)
{
	AssertLocked();
	if (label_position == fAppearance.fLabelPosition ||
	  (label_position != W_TOOLBAR_LABEL_BOTTOM &&
	  label_position != W_TOOLBAR_LABEL_SIDE &
	  label_position != W_TOOLBAR_LABEL_NONE))
		return;
	fAppearance.fLabelPosition = label_position;
	Update();
}

void WToolbar::SetMargin(float margin)
{
	AssertLocked();
	if (margin == fAppearance.fMargin || floor(margin) < 0.0) return;
	fAppearance.fMargin = floor(margin);
	Update();
}

void WToolbar::SetPadding(float padding)
{
	AssertLocked();
	if (padding == fAppearance.fPadding || floor(padding) < 0.0) return;
	fAppearance.fPadding = floor(padding);
	Update();
}

void WToolbar::SetPictureSize(float picture_size)
{
	AssertLocked();
	if (picture_size == fAppearance.fPicSize || picture_size < 0.0 ||
	  picture_size > static_cast<float>(W_TOOLBAR_PICTURE_MAX))
	  	return;
	fAppearance.fPicSize = picture_size;
	Update();
}

void WToolbar::SetStyle(int style)
{
	AssertLocked();
	fAppearance.fStyle = style;
	fDisableUpdate = true;
	switch (style) {
		case W_TOOLBAR_STYLE_FLAT:
			fAppearance.fStyle = W_TOOLBAR_STYLE_FLAT;
			if (fDisableStyling) break;
			fAppearance.fLabelPosition = W_TOOLBAR_LABEL_NONE;
			fAppearance.fMargin = 0.0;
			fAppearance.fPadding = 2.0;
			fAppearance.fPicSize =
				static_cast<float>(W_TOOLBAR_PICTURE_XSMALL);
			SetLowColor(ui_color(B_PANEL_BACKGROUND_COLOR));
			SetFont(be_plain_font);
			SetHighColor(0, 0, 0);
			break;
		case W_TOOLBAR_STYLE_3D:
			fAppearance.fStyle = W_TOOLBAR_STYLE_3D;
			if (fDisableStyling) break;
			fAppearance.fLabelPosition = W_TOOLBAR_LABEL_NONE;
			fAppearance.fMargin = 0.0;
			fAppearance.fPadding = 2.0;
			fAppearance.fPicSize =
				static_cast<float>(W_TOOLBAR_PICTURE_XSMALL);
			SetLowColor(ui_color(B_PANEL_BACKGROUND_COLOR));
			SetFont(be_plain_font);
			SetHighColor(0, 0, 0);
			break;
		case W_TOOLBAR_STYLE_MENU: {
			fAppearance.fStyle = W_TOOLBAR_STYLE_MENU;
			if (fDisableStyling) break;
			menu_info mi;
			BFont font;
			get_menu_info(&mi);
			fAppearance.fLabelPosition = W_TOOLBAR_LABEL_SIDE;
			fAppearance.fMargin = 0.0;
			fAppearance.fPadding = 0.0;
			fAppearance.fPicSize =
				static_cast<float>(W_TOOLBAR_PICTURE_XSMALL);
			SetLowColor(mi.background_color);
			// BeBook says that menu_info has a property called "font",
			// but that's wrong; it has "f_family" and "f_style" instead.
			font = be_plain_font;
			font.SetFamilyAndStyle(mi.f_family, mi.f_style);
			font.SetSize(mi.font_size);
			SetFont(&font);
			SetHighColor(0, 0, 0);
			} break;
		default:
			return;
	}
	fDisableUpdate = false;
	Update();
}

int WToolbar::Style(void)
{
	return fAppearance.fStyle;
}

// Target - mimic BInvoker

bool WToolbar::IsTargetLocal(void) const
{
	return fTarget.IsTargetLocal();
}

BMessenger WToolbar::Messenger(void) const
{
	return fTarget;
}

status_t WToolbar::SetControlsTarget(BMessenger messenger)
{
	AssertLocked();
	const unsigned kItems = fItems.size();
	WToolbarControl *control;
	unsigned i;
	for (i = 0; i < kItems; i++) {
		control = dynamic_cast<WToolbarControl*>(fItems[i]);
		if (control != NULL)
			control->SetTarget(messenger);
	}
	return B_OK;
}

status_t WToolbar::SetControlsTarget(const BHandler *handler,
	const BLooper *looper)
{
	AssertLocked();
	const unsigned kItems = fItems.size();
	WToolbarControl *control;
	unsigned i;
	if (handler == NULL && looper == NULL)
		return B_BAD_VALUE;
	if (handler != NULL) {
		if (handler->Looper() == NULL)
			return B_BAD_VALUE;
		if (looper != NULL && handler->Looper() != looper)
			return B_MISMATCHED_VALUES;
	}
	for (i = 0; i < kItems; i++) {
		control = dynamic_cast<WToolbarControl*>(fItems[i]);
		if (control != NULL)
			control->SetTarget(handler, looper);
	}
	return B_OK;
}

status_t WToolbar::SetTarget(BMessenger messenger)
{
	AssertLocked();
	fTarget = messenger;
	return B_OK;
}

status_t WToolbar::SetTarget(const BHandler *handler, const BLooper *looper)
{
	AssertLocked();
	if (handler == NULL && looper == NULL)
		return B_BAD_VALUE;
	if (handler != NULL) {
		if (handler->Looper() == NULL)
			return B_BAD_VALUE;
		if (looper != NULL && handler->Looper() != looper)
			return B_MISMATCHED_VALUES;
	}
	fTarget = BMessenger(handler, looper);
	return B_OK;
}

BHandler * WToolbar::Target(BLooper **looper) const
{
	return fTarget.Target(looper);
}

// Other methods

void WToolbar::Update(void)
{
	if (fDisableUpdate) return;
	AssertLocked();

	float border, left, top, max, width, height, line, realLine;
	WToolbarItem *item;
	unsigned i;
	BRect r;

	// Search for the largest (or tallest) item
	max = 0.0;
	for (i = 0; i < fItems.size(); i++) {
		if (fItems[i]->fVisible) {
			if (fAppearance.fAlignment == W_TOOLBAR_VERTICAL) {
				fItems[i]->GetPreferredSize(&width, NULL);
				if (width + 1.0 > max) max = width + 1.0;
			}
			else {
				fItems[i]->GetPreferredSize(NULL, &height);
				if (height + 1.0 > max) max = height + 1.0;
			}
		}
	}

	// Assign each item its rectangle
	switch (fAppearance.fBorder) {
		case B_FANCY_BORDER:
			border = BorderSize();
			break;
		case B_PLAIN_BORDER:
			border = 1.0;
			break;
		default:
			border = 0.0;
			break;
	}
	left = border + fAppearance.fPadding + fAppearance.fMargin;
	top = border + fAppearance.fPadding + fAppearance.fMargin;
	realLine = 0;
	line = -1;
	for (i = 0; i < fItems.size(); i++) {
		item = fItems[i];
		if (item->fVisible) {
			if (line == -1)
				line = item->fLine;
			if (fAppearance.fAlignment == W_TOOLBAR_VERTICAL) {
				if (item->fLine != line) {
					line = item->fLine;
					realLine++;
					top = border + fAppearance.fPadding + fAppearance.fMargin;
					left = border + fAppearance.fPadding + max * realLine +
						fAppearance.fMargin * (realLine + 1.0);
				}
				r.top = top;
				r.left = left;
				fItems[i]->GetPreferredSize(NULL, &height);
				r.bottom = r.top + height;
				r.right = r.left + max - 1.0;
				top = r.bottom + fAppearance.fMargin + 1.0;
			}
			else {
				if (item->fLine != line) {
					line = item->fLine;
					realLine++;
					top = border + fAppearance.fPadding + max * realLine +
						fAppearance.fMargin * (realLine + 1.0);
					left = border + fAppearance.fPadding + fAppearance.fMargin;
				}
				r.top = top;
				r.left = left;
				fItems[i]->GetPreferredSize(&width, NULL);
				r.bottom = r.top + max - 1.0;
				r.right = r.left + width;
				left = r.right + fAppearance.fMargin + 1.0;
			}
			fItems[i]->fFrame = r;
		}
	}

	// Auto-resize the toolbar
	if (fAutoSize) {
		// TODO resize accordingly to BView's resizing mode
		float w, h;
		GetPreferredSize(&w, &h);
		if (w != Frame().Width() || h != Frame().Height())
			ResizeToPreferred();
		else {
			if (Window() != NULL)
				Invalidate();
		}
	}
	else {
		if (Window() != NULL)
			Invalidate();
	}

	// If the rects have changed the mouse may be over a different
	// item than before, so we have to simulate a MouseOver() event
	if (Window() != NULL) {
		WToolbarItem *new_over;
		uint32 buttons;
		BPoint point;
		GetMouse(&point, &buttons, false);
		new_over = ItemAt(point);
		if (fMouseDownItem == NULL) {
			if (new_over != fMouseOverItem) {
				if (fMouseOverItem != NULL) {
					fMouseOverItem->MouseMoved(
						point - fMouseOverItem->fFrame.LeftTop(),
						B_EXITED_VIEW, NULL);
				}
				if (new_over != NULL) {
					new_over->MouseMoved(point - new_over->fFrame.LeftTop(),
						B_ENTERED_VIEW, NULL);
				}
				fMouseOverItem = new_over;
			}
			else {
				if (fMouseOverItem != NULL) {
					fMouseOverItem->MouseMoved(
						point - fMouseOverItem->fFrame.LeftTop(),
						B_INSIDE_VIEW, NULL);
				}
			}
		}
		else {
			if (new_over != fMouseDownItem)
				fMouseDownItem->MouseMoved(point - fMouseDownItem->fFrame.LeftTop(),
					B_EXITED_VIEW, NULL);
			else
				fMouseDownItem->MouseMoved(point - fMouseDownItem->fFrame.LeftTop(),
					B_INSIDE_VIEW, NULL);
		}
	}
}
