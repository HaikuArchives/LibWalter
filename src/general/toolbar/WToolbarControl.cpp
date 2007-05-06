// =============================================================================
//
// libwalter WToolbarControl.cpp
//
// Base class for WToolbar controls
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

/*!
 * \class WToolbarControl
 * \brief Base class for toolbar controls
 *
 * WToolbarControl provides all the basic facilities to create an archivable
 * and invokable toolbar control.
 *
 * \par How to create your own control
 * Creating your own control by overloading WToolbarControl is not very
 * different than overloading BControl. The most obvious methods you have to
 * overload are Draw() and GetPreferredSize(). If your control needs to send a
 * message after user' action, you may also want to overload MouseUp().
 */

// libwalter headers
#include "WToolbar.h"
#include "WToolbarControl.h"

// =============================================================================
// WToolbarControl
// =============================================================================

// Constructors and destructors

WToolbarControl::WToolbarControl(const char *name, BMessage *message) :
	BHandler(name),
	BInvoker(message, NULL)
{
	_init_object();
	SetName(name);
}

WToolbarControl::WToolbarControl(BMessage *archive) :
	BHandler(archive)
{
	bool enabled, visible;
	BMessage message;
	int line;

	_init_object();

	if (archive->FindMessage("WToolbarControl::message", &message) == B_OK)
		SetMessage(new BMessage(message));

	if (archive->FindBool("WToolbarControl::enabled", &enabled) == B_OK)
		SetEnabled(enabled);

	if (archive->FindInt32("WToolbarControl::line", (int32*)(&line)) == B_OK)
		fLine = line;

	if (archive->FindBool("WToolbarControl::visible", &visible) == B_OK)
		SetVisible(visible);
}

WToolbarControl::~WToolbarControl()
{
	if (fToolbar != NULL)
		fToolbar->RemoveControl(this);
}

// Private

void WToolbarControl::_init_object(void)
{
	fEnabled = true;
	fFrame = BRect(0.0, 0.0, -1.0, -1.0);
	fHeight = 0.0;
	fLine = -1;
	fMouseDown = false;
	fMouseOver = false;
	fToolbar = NULL;
	fVisible = true;
	fWidth = 0.0;
}

// BArchivable hooks

status_t WToolbarControl::Archive(BMessage *archive, bool deep) const
{
	status_t status;

	// Ancestor
	status = BHandler::Archive(archive, deep);

	// Properties

	if (status == B_OK && Message() != NULL)
		status = archive->AddMessage("WToolbarControl::message", Message());

	if (status == B_OK)
		status = archive->AddBool("WToolbarControl::enabled", fEnabled);

	if (status == B_OK)
		status = archive->AddInt32("WToolbarControl::line", (int32)fLine);

	if (status == B_OK)
		status = archive->AddBool("WToolbarControl::visible", fVisible);

	return status;
}

BArchivable * WToolbarControl::Instantiate(BMessage *archive)
{
	return (validate_instantiation(archive, "WToolbarControl") ?
		new WToolbarControl(archive) : NULL);
}

// Public

void WToolbarControl::AttachedToToolbar(void)
{
	if (!(Messenger().IsValid()))
		SetTarget(fToolbar->Messenger());
}

BRect WToolbarControl::Bounds(void)
{
	return fFrame.OffsetToCopy(0.0, 0.0);
}

void WToolbarControl::DetachedFromToolbar(void)
{
}

void WToolbarControl::Draw(BView *canvas, BRect update_rect)
{
	// The basic class doesn't draw anything
}

bool WToolbarControl::Enabled(void)
{
	return fEnabled;
}

BRect WToolbarControl::Frame(void)
{
	return fFrame;
}

void WToolbarControl::GetPreferredSize(float *width, float *height)
{
	if (width != NULL) *width = -1.0;
	if (height != NULL) *height = -1.0;
}

void WToolbarControl::Invalidate(void)
{
	if (fToolbar == NULL) return;
	// If size has changed, we've to tell the toolbar to recalculate the
	// control rects, otherwise just to redraw ourself
	float width, height;
	GetPreferredSize(&width, &height);
	if (width != fWidth || height != fHeight) {
		fWidth = width;
		fHeight = height;
		fToolbar->Update();
	}
	else
		fToolbar->DrawControl(this);
}

int WToolbarControl::Line(void)
{
	return (fToolbar == NULL ? -1 : fLine);
}

void WToolbarControl::MouseDown(BPoint point)
{
	if (fEnabled && fMouseOver)
		fMouseDown = true;
	Invalidate();
}

void WToolbarControl::MouseMoved(BPoint point, uint32 transit,
	const BMessage *message)
{
	bool old_mouse_over = fMouseOver;
	fMouseOver = (transit == B_ENTERED_VIEW || transit == B_INSIDE_VIEW);
	if (fMouseOver != old_mouse_over)
		Invalidate();
}

void WToolbarControl::MouseUp(BPoint point)
{
	fMouseDown = false;
	Invalidate();
}

int WToolbarControl::Position(void)
{
	if (fToolbar == NULL) return -1;
	int first, pos = 0;
	first = fToolbar->IndexOf(fToolbar->ControlAt(fLine, 0));
	while (fToolbar->ControlAt(first + pos) != this) {
		pos++;
	}
	return pos;
}

void WToolbarControl::SetEnabled(bool enabled)
{
	if (fEnabled == enabled) return;
	fEnabled = enabled;
	if (!fEnabled)
		fMouseDown = false;
	Invalidate();
}

void WToolbarControl::SetVisible(bool visible)
{
	if (fVisible == visible) return;
	fVisible = visible;
	if (!fVisible) {
		fMouseDown = false;
		fMouseOver = false;
	}
	if (fToolbar != NULL)
		fToolbar->Update();
}

WToolbar * WToolbarControl::Toolbar(void)
{
	return fToolbar;
}

bool WToolbarControl::Visible(void)
{
	return fVisible;
}
