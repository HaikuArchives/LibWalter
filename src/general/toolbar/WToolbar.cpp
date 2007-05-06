// =============================================================================
//
// libwalter WToolbar.Ccpp
//
// WToolbar, a toolbar widget
//
// Revision: 20070501
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
#include "WToolbar.h"
#include "WToolbarControl.h"
#include "WToolbarSupport.h"

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
	WToolbarControl *control;
	BArchivable *archivable;
	bool autoSize, enabled;
	border_style border;
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

	// Controls

	index = 0;
	while (archive->FindMessage("WToolbar::control", index,
	  &message) == B_OK) {
		archivable = instantiate_object(&message);
		if (archivable != NULL) {
			control = dynamic_cast<WToolbarControl*>(archivable);
			if (control != NULL) {
				if (!AddControl(control, control->fLine))
					delete control;
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

	// Delete all the controls
	WToolbarControl *control;
	while (fControls.size() > 0) {
		control = fControls[0];
		RemoveControl(control);
		delete control;
	}

	// Other cleanups
	if (fAppearance.fBackBitmap != NULL)
		delete fAppearance.fBackBitmap;
}

// Private

void WToolbar::_draw_control(unsigned index, BRect updateRect)
{
	// Beware, this function does not perform any sanity checking!!!
	// TODO set the clipping region and update rect
	BPoint origin;
	origin = Origin();
	SetOrigin(fControls[index]->Frame().LeftTop());
	PushState();
	fControls[index]->Draw(this, fControls[index]->Bounds());
	PopState();
	SetOrigin(origin);
	Sync();
}

void WToolbar::_init_object(void)
{
	// Internals
	fDisableUpdate = true;
	fDisableStyling = false;
	fDownControl = NULL;
	fOverControl = NULL;

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

	// Archive controls

	if (deep) {
		const unsigned kControls = fControls.size();
		unsigned i;
		for (i = 0; i < kControls; i++) {
			if (status == B_OK) {
				BMessage control;
				status = fControls[i]->Archive(&control, true);
				if (status == B_OK)
					status = archive->AddMessage("WToolbar::control",
						&control);
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
		const unsigned kControls = fControls.size();
		WToolbarControl *control;
		unsigned i;
		SetTarget(Window());
		for (i = 0; i < kControls; i++) {
			control = fControls[i];
			if (!(control->Messenger().IsValid())) {
				control->SetTarget(fTarget);
			}
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

	for (unsigned i = 0; i < fControls.size(); i++) {
		if (fControls[i]->fFrame.Intersects(updateRect) &&
		  fControls[i]->fVisible)
			_draw_control(i, updateRect);
	}
}

void WToolbar::GetPreferredSize(float *width, float *height)
{
	unsigned i, visible_controls, visible_lines;
	float w = 0.0, h = 0.0, max;
	WToolbarControl *control;
	int line;

	// Count visible controls
	visible_controls = 0;
	if (fControls.size() > 0) {
		for (i = 0; i < fControls.size(); i++) {
			if (fControls[i]->fVisible)
				visible_controls++;
		}
	}

	// Calculate width and height
	if (visible_controls > 0) {
		line = 0;
		visible_lines = 1;
		max = 0.0;
		if (fAppearance.fAlignment == W_TOOLBAR_VERTICAL) {
			// Height is the height of the tallest line, line height is the sum
			// of the heights of its visible controls
			// Width is the width of any of the control times the visible lines
			for (i = 0; i < fControls.size(); i++) {
				control = fControls[i];
				if (control->fVisible) {
					if (control->fLine != line) {
						visible_lines++;
						line = control->fLine;
						if (h > max)
							max = h;
						h = 0.0;
					}
					h += control->fFrame.Height() + 1.0 + fAppearance.fMargin;
					if (w == 0.0)
						w = fControls[i]->fFrame.Width() + 1.0;
				}
			}
			h = (h > max ? h : max) + fAppearance.fMargin;
			w = fAppearance.fMargin + (w + fAppearance.fMargin) *
				static_cast<float>(visible_lines);
		}
		else {
			// Width is the width of the widest line, line width is the sum
			// of the widths of its visible controls
			// Height is the height of any of the control times the visible
			// lines
			for (i = 0; i < fControls.size(); i++) {
				control = fControls[i];
				if (control->fVisible) {
					if (control->fLine != line) {
						visible_lines++;
						line = control->fLine;
						if (w > max)
							max = w;
						w = 0.0;
					}
					w += control->fFrame.Width() + 1.0 + fAppearance.fMargin;
					if (h == 0.0)
						h = fControls[i]->fFrame.Height() + 1.0;
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

		// If the mouse is over a control, tell the control about this
		if (fOverControl != NULL && fEnabled) {
			fOverControl->MouseDown(point - fOverControl->fFrame.LeftTop());
			fDownControl = fOverControl;
		}
	}
}

void WToolbar::MouseMoved(BPoint point, uint32 transit,
	const BMessage *message)
{
	if (Looper() == NULL) return;

	if (fDownControl == NULL) {

		int32 buttons = 0;
		BMessage *msg;

		// Pressed mouse buttons
		msg = Looper()->CurrentMessage();
		if (msg != NULL)
			msg->FindInt32("buttons", &buttons);

		if (buttons == 0) {
			if (transit == B_ENTERED_VIEW || transit == B_INSIDE_VIEW) {
				WToolbarControl *old_over = fOverControl;
				fOverControl = ControlAt(point);
				if (fOverControl != old_over) {
					// Remove mouse over from previous control
					if (old_over != NULL)
						old_over->MouseMoved(point - old_over->fFrame.LeftTop(),
							B_EXITED_VIEW, message);
					// Assign mouse over to new control
					if (fOverControl != NULL)
						fOverControl->MouseMoved(
							point - fOverControl->fFrame.LeftTop(),
							B_ENTERED_VIEW, message);
				}
				else {
					// Tell the control that the mouse has moved
					if (fOverControl != NULL)
						fOverControl->MouseMoved(
							point - fOverControl->fFrame.LeftTop(),
							B_INSIDE_VIEW, message);
				}
			}
			else {
				// B_EXITED_VIEW or B_OUTSIDE_VIEW, remove focus
				if (fOverControl != NULL) {
					fOverControl->MouseMoved(
						point - fOverControl->fFrame.LeftTop(),
						B_EXITED_VIEW, message);
					fOverControl = NULL;
				}
			}
		}
	}

	// If the mouse is down, we need to remove the mouse over from the currently
	// pressed control, but not assign it to a new one.
	else {
		if (transit == B_ENTERED_VIEW || transit == B_INSIDE_VIEW) {
			WToolbarControl *new_over = ControlAt(point);
			if (new_over != fDownControl) {
				// Remove mouse over from pressed control
				fDownControl->MouseMoved(point - fDownControl->fFrame.LeftTop(),
					B_EXITED_VIEW, message);
			}
			else {
				// Tell the control that the mouse has moved
				fDownControl->MouseMoved(point - fDownControl->fFrame.LeftTop(),
					B_INSIDE_VIEW, message);
			}
		}
		else {
			// B_EXITED_VIEW or B_OUTSIDE_VIEW, remove focus
			fDownControl->MouseMoved(point - fDownControl->fFrame.LeftTop(),
				B_EXITED_VIEW, message);
		}
	}
}

void WToolbar::MouseUp(BPoint point)
{
	if (Looper() == NULL) return;

	WToolbarControl *new_over;
	BMessage *msg;

	msg = Looper()->CurrentMessage();

	// If a control is pressed, tell it what is happening
	if (fDownControl != NULL) {
		fDownControl->MouseUp(point - fDownControl->fFrame.LeftTop());
		fDownControl = NULL;
	}

	// Check which control the mouse is over
	new_over = ControlAt(point);
	if (new_over != fOverControl) {
		if (fOverControl != NULL)
			fOverControl->MouseMoved(point - fOverControl->fFrame.LeftTop(),
				B_EXITED_VIEW, msg);
		if (new_over != NULL)
			new_over->MouseMoved(point - new_over->fFrame.LeftTop(),
				B_ENTERED_VIEW, msg);
		fOverControl = new_over;
	}
	else {
		if (fOverControl != NULL)
			fOverControl->MouseMoved(point - fOverControl->fFrame.LeftTop(),
				B_INSIDE_VIEW, msg);
	}
}

// Controls management

bool WToolbar::AddControl(WToolbarControl *control, int line, int position)
{
	// Controls are stored by line, then by position.

	AssertLocked();
	if (control->Toolbar() != NULL)
		return (control->Toolbar() == this);
	if (fControls.size() >= W_TOOLBAR_MAX_CONTROLS)
		return false;

	bool disable_update = fDisableUpdate;
	fDisableUpdate = true;

	if (line >= 0 && line <= CountLines()) {
		// Insert the control at the given line and, if possible, position
		WToolbarControls::iterator i;
		unsigned _first = 0, _pos = 0;
		// Find the first control of the line
		while (_first < fControls.size()) {
			if (fControls[_first]->fLine == line) break;
			_first++;
		}
		// Find the control at position, if any
		while (_first + _pos < fControls.size()) {
			if ((int)_pos == position) break;
			if (fControls[_first + _pos]->fLine == line + 1) break;
			_pos++;
		}
		// Insert the control
		control->fLine = line;
		i = fControls.begin() + _first + _pos;
		fControls.insert(i, control);
	}
	else {
		// Insert the control at the end of the last line
		control->fLine = (CountLines() > 0 ? CountLines() - 1 : 0);
		fControls.push_back(control);
	}

	control->fToolbar = this;
	control->AttachedToToolbar();

	fDisableUpdate = disable_update;
	Update();
	return true;
}

WToolbarControl * WToolbar::ControlAt(BPoint point)
{
	if (fControls.size() == 0) return NULL;
	unsigned i = 0;
	while (i < fControls.size()) {
		if (fControls[i]->fVisible && fControls[i]->fFrame.Contains(point))
			return fControls[i];
		i++;
	}
	return NULL;
}

WToolbarControl * WToolbar::ControlAt(int index)
{
	return (index >= 0 && index < (int)(fControls.size()) ?
		fControls[(unsigned)index] : NULL);
}

WToolbarControl * WToolbar::ControlAt(int line, int position, bool exact)
{
	if (fControls.size() == 0)
		return NULL;

	// If exact, exclude impossible values
	if (exact && (position < 0 || position >= (int)(fControls.size()) ||
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

	// Find the first control of the line
	while (_first < fControls.size()) {
		if (fControls[_first]->fLine == _line) break;
		_first++;
	}

	// Look for the requested control
	if (position > 0) {
		while (true) {
			if (fControls[_first + _pos]->fLine != _line) {
				if (exact) return NULL;
				_pos--;
				break;
			}
			if ((int)_pos == position) break;
			if (_first + _pos == fControls.size() - 1) break;
			_pos++;
		}
	}

	return fControls[_first + _pos];
}

int WToolbar::ControlsInLine(int line)
{
	if (line < 0) return 0;
	int count = 0;
	for (unsigned i = 0; i < fControls.size(); i++) {
		if (fControls[i]->fLine == line) count++;
	}
	return count;
}

int WToolbar::CountControls(void)
{
	return (int)(fControls.size());
}

int WToolbar::CountLines(void)
{
	int lines = -1;
	for (unsigned i = 0; i < fControls.size(); i++) {
		if (fControls[i]->fLine > lines)
			lines = fControls[i]->fLine;
	}
	return lines + 1;
}

bool WToolbar::DrawControl(WToolbarControl *control)
{
	AssertLocked();
	if (control == NULL) return false;
	int index = IndexOf(control);
	if (index == -1) return false;
	if (Window() == NULL) return true;
	Invalidate(control->Frame());
	return true;
}

WToolbarControl * WToolbar::FindControl(const char *name)
{
	unsigned i = 0;
	while (i < fControls.size()) {
		if (name == NULL) {
			if (fControls[i]->Name() == NULL) return fControls[i];
		}
		else
			if (strcmp(name, fControls[i]->Name()) == 0) return fControls[i];
		i++;
	}
	return NULL;
}

int WToolbar::IndexOf(WToolbarControl *control)
{
	if (control == NULL) return -1;
	if (control->Toolbar() != this) return -1;
	unsigned i = 0;
	while (i < fControls.size()) {
		if (fControls[i] == control) return (int)i;
		i++;
	}
	return -1;
}

bool WToolbar::RemoveControl(WToolbarControl *control)
{
	if (control == NULL) return false;

	AssertLocked();

	bool disable_update;
	int index, line;

	// Index of the control
	index = IndexOf(control);
	if (index == -1) return false;

	// Make sure no updates are performed
	disable_update = fDisableUpdate;
	fDisableUpdate = true;

	// Remove the control
	if (fDownControl == control)
		fDownControl = NULL;
	line = control->fLine;
	control->fToolbar = NULL;
	control->fFrame = BRect(0.0, 0.0, -1.0, -1.0);
	control->fLine = -1;
	control->DetachedFromToolbar();
	fControls.erase(fControls.begin() + (unsigned)index);

	// Make sure we don't leave empty lines
	if (ControlsInLine(line) == 0) {
		while (fControls.size() > (unsigned)index) {
			fControls[index]->fLine--;
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
	const unsigned kControls = fControls.size();
	unsigned i;
	for (i = 0; i < kControls; i++)
		fControls[i]->SetTarget(messenger);
	return B_OK;
}

status_t WToolbar::SetControlsTarget(const BHandler *handler,
	const BLooper *looper)
{
	AssertLocked();
	const unsigned kControls = fControls.size();
	unsigned i;
	if (handler == NULL && looper == NULL)
		return B_BAD_VALUE;
	if (handler != NULL) {
		if (handler->Looper() == NULL)
			return B_BAD_VALUE;
		if (looper != NULL && handler->Looper() != looper)
			return B_MISMATCHED_VALUES;
	}
	for (i = 0; i < kControls; i++)
		fControls[i]->SetTarget(handler, looper);
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
	WToolbarControl *control;
	unsigned i;
	BRect r;

	// Search for the largest (or tallest) control
	max = 0.0;
	for (i = 0; i < fControls.size(); i++) {
		if (fControls[i]->fVisible) {
			if (fAppearance.fAlignment == W_TOOLBAR_VERTICAL) {
				fControls[i]->GetPreferredSize(&width, NULL);
				if (width + 1.0 > max) max = width + 1.0;
			}
			else {
				fControls[i]->GetPreferredSize(NULL, &height);
				if (height + 1.0 > max) max = height + 1.0;
			}
		}
	}

	// Assign each control its rectangle
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
	for (i = 0; i < fControls.size(); i++) {
		control = fControls[i];
		if (control->fVisible) {
			if (line == -1)
				line = control->fLine;
			if (fAppearance.fAlignment == W_TOOLBAR_VERTICAL) {
				if (control->fLine != line) {
					line = control->fLine;
					realLine++;
					top = border + fAppearance.fPadding + fAppearance.fMargin;
					left = border + fAppearance.fPadding + max * realLine +
						fAppearance.fMargin * (realLine + 1.0);
				}
				r.top = top;
				r.left = left;
				fControls[i]->GetPreferredSize(NULL, &height);
				r.bottom = r.top + height;
				r.right = r.left + max - 1.0;
				top = r.bottom + fAppearance.fMargin + 1.0;
			}
			else {
				if (control->fLine != line) {
					line = control->fLine;
					realLine++;
					top = border + fAppearance.fPadding + max * realLine +
						fAppearance.fMargin * (realLine + 1.0);
					left = border + fAppearance.fPadding + fAppearance.fMargin;
				}
				r.top = top;
				r.left = left;
				fControls[i]->GetPreferredSize(&width, NULL);
				r.bottom = r.top + max - 1.0;
				r.right = r.left + width;
				left = r.right + fAppearance.fMargin + 1.0;
			}
			fControls[i]->fFrame = r;
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
	// control than before, so we have to simulate a MouseOver() event
	if (Window() != NULL) {
		WToolbarControl *new_over;
		uint32 buttons;
		BPoint point;
		GetMouse(&point, &buttons, false);
		new_over = ControlAt(point);
		if (fDownControl == NULL) {
			if (new_over != fOverControl) {
				if (fOverControl != NULL) {
					fOverControl->MouseMoved(
						point - fOverControl->fFrame.LeftTop(),
						B_EXITED_VIEW, NULL);
				}
				if (new_over != NULL) {
					new_over->MouseMoved(point - new_over->fFrame.LeftTop(),
						B_ENTERED_VIEW, NULL);
				}
				fOverControl = new_over;
			}
			else {
				if (fOverControl != NULL) {
					fOverControl->MouseMoved(
						point - fOverControl->fFrame.LeftTop(),
						B_INSIDE_VIEW, NULL);
				}
			}
		}
		else {
			if (new_over != fDownControl)
				fDownControl->MouseMoved(point - fDownControl->fFrame.LeftTop(),
					B_EXITED_VIEW, NULL);
			else
				fDownControl->MouseMoved(point - fDownControl->fFrame.LeftTop(),
					B_INSIDE_VIEW, NULL);
		}
	}
}
