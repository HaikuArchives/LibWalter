// =============================================================================
//
// libwalter ToolbarControl.cpp
//
// Base class for Toolbar controls
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
 * \class ToolbarControl
 * \brief Base class for toolbar controls
 *
 * ToolbarControl provides all the basic facilities to create an archivable
 * and invokable toolbar control.
 *
 * \par How to create your own control
 * Creating your own control by overloading ToolbarControl is not very
 * different than overloading BControl. The most obvious methods you have to
 * overload are Draw() and GetPreferredSize(). If your control needs to send a
 * message after user' action, you may also want to overload MouseUp().
 */

// libwalter headers
#include "Toolbar.h"
#include "ToolbarControl.h"

// =============================================================================
// ToolbarControl
// =============================================================================

// Constructors and destructors

ToolbarControl::ToolbarControl(const char *name, BMessage *message) :
	BHandler(name),
	BInvoker(message, NULL)
{
	_init_object();
	SetName(name);
}

ToolbarControl::ToolbarControl(BMessage *archive) :
	BHandler(archive)
{
	bool enabled, visible;
	BMessage message;
	int line;

	_init_object();

	if (archive->FindMessage("ToolbarControl::message", &message) == B_OK)
		SetMessage(new BMessage(message));

	if (archive->FindBool("ToolbarControl::enabled", &enabled) == B_OK)
		SetEnabled(enabled);

	if (archive->FindInt32("ToolbarControl::line", (int32*)(&line)) == B_OK)
		fLine = line;

	if (archive->FindBool("ToolbarControl::visible", &visible) == B_OK)
		SetVisible(visible);
}

ToolbarControl::~ToolbarControl()
{
	if (fToolbar != NULL)
		fToolbar->RemoveControl(this);
}

// Private

void ToolbarControl::_init_object(void)
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

status_t ToolbarControl::Archive(BMessage *archive, bool deep) const
{
	status_t status;

	// Ancestor
	status = BHandler::Archive(archive, deep);

	// Properties

	if (status == B_OK && Message() != NULL)
		status = archive->AddMessage("ToolbarControl::message", Message());

	if (status == B_OK)
		status = archive->AddBool("ToolbarControl::enabled", fEnabled);

	if (status == B_OK)
		status = archive->AddInt32("ToolbarControl::line", (int32)fLine);

	if (status == B_OK)
		status = archive->AddBool("ToolbarControl::visible", fVisible);

	return status;
}

BArchivable * ToolbarControl::Instantiate(BMessage *archive)
{
	return (validate_instantiation(archive, "ToolbarControl") ?
		new ToolbarControl(archive) : NULL);
}

// Public

void ToolbarControl::AttachedToToolbar(void)
{
	if (!(Messenger().IsValid()))
		SetTarget(fToolbar->Messenger());
}

BRect ToolbarControl::Bounds(void)
{
	return fFrame.OffsetToCopy(0.0, 0.0);
}

void ToolbarControl::DetachedFromToolbar(void)
{
}

void ToolbarControl::Draw(BView *canvas, BRect update_rect)
{
	// The basic class doesn't draw anything
}

bool ToolbarControl::Enabled(void)
{
	return fEnabled;
}

BRect ToolbarControl::Frame(void)
{
	return fFrame;
}

void ToolbarControl::GetPreferredSize(float *width, float *height)
{
	if (width != NULL) *width = -1.0;
	if (height != NULL) *height = -1.0;
}

void ToolbarControl::Invalidate(void)
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

int ToolbarControl::Line(void)
{
	return (fToolbar == NULL ? -1 : fLine);
}

void ToolbarControl::MouseDown(BPoint point)
{
	if (fEnabled && fMouseOver)
		fMouseDown = true;
	Invalidate();
}

void ToolbarControl::MouseMoved(BPoint point, uint32 transit,
	const BMessage *message)
{
	bool old_mouse_over = fMouseOver;
	fMouseOver = (transit == B_ENTERED_VIEW || transit == B_INSIDE_VIEW);
	if (fMouseOver != old_mouse_over)
		Invalidate();
}

void ToolbarControl::MouseUp(BPoint point)
{
	fMouseDown = false;
	Invalidate();
}

int ToolbarControl::Position(void)
{
	if (fToolbar == NULL) return -1;
	int first, pos = 0;
	first = fToolbar->IndexOf(fToolbar->ControlAt(fLine, 0));
	while (fToolbar->ControlAt(first + pos) != this) {
		pos++;
	}
	return pos;
}

void ToolbarControl::SetEnabled(bool enabled)
{
	if (fEnabled == enabled) return;
	fEnabled = enabled;
	if (!fEnabled)
		fMouseDown = false;
	Invalidate();
}

void ToolbarControl::SetVisible(bool visible)
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

Toolbar * ToolbarControl::ParentToolbar(void)
{
	return fToolbar;
}

bool ToolbarControl::Visible(void)
{
	return fVisible;
}
