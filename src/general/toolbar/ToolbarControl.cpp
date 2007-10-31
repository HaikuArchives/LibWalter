// =============================================================================
//
// libwalter ToolbarControl.cpp
//
// Base class for WToolbar controls
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

/*!
 * \class WToolbarControl
 * \brief Base class for toolbar controls
 *
 * WToolbarControl provides all the basic facilities to create an archivable
 * and invokable toolbar control. It is a descendant of WToolbarItem.
 *
 * libwalter supplies several widely used toolbar controls and items, such as
 * WToolbarSeparator and WToolbarButton, and many more.
 *
 * \par How to create your own control
 * Creating your own control by overloading WToolbarControl is not very
 * different than overloading BControl. The most obvious methods you have to
 * overload are Draw() and GetPreferredSize(). If your control needs to send a
 * message after user' action, you may also want to overload MouseUp().
 */

// libwalter headers
#include "Toolbar.h"
#include "ToolbarControl.h"

// =============================================================================
// WToolbarControl
// =============================================================================

// Constructors and destructors

WToolbarControl::WToolbarControl(const char *name, BMessage *message) :
	WToolbarItem(name),
	BInvoker(message, NULL)
{
	_init_object();
}

WToolbarControl::WToolbarControl(BMessage *archive) :
	WToolbarItem(archive),
	BInvoker(NULL, NULL)
{
	bool enabled, visible;
	BMessage message;
	int line;

	_init_object();

	if (archive->FindMessage("WToolbarControl::message", &message) == B_OK)
		SetMessage(new BMessage(message));

	if (archive->FindBool("WToolbarControl::enabled", &enabled) == B_OK)
		SetEnabled(enabled);
}

WToolbarControl::~WToolbarControl()
{
}

// Private

void WToolbarControl::_init_object(void)
{
	fEnabled = true;
}

// BArchivable hooks

status_t WToolbarControl::Archive(BMessage *archive, bool deep) const
{
	status_t status;

	// Ancestor
	status = WToolbarItem::Archive(archive, deep);

	// Properties

	if (status == B_OK && Message() != NULL)
		status = archive->AddMessage("WToolbarControl::message", Message());

	if (status == B_OK)
		status = archive->AddBool("WToolbarControl::enabled", fEnabled);

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
		SetTarget(Toolbar()->Messenger());
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

void WToolbarControl::GetPreferredSize(float *width, float *height)
{
	if (width != NULL) *width = -1.0;
	if (height != NULL) *height = -1.0;
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
	WToolbarItem::MouseMoved(point, transit, message);
}

void WToolbarControl::MouseUp(BPoint point)
{
	fMouseDown = false;
	Invalidate();
}

void WToolbarControl::SetEnabled(bool enabled)
{
	if (fEnabled == enabled) return;
	fEnabled = enabled;
	if (!fEnabled)
		fMouseDown = false;
	Invalidate();
}

void WToolbarControl::Update(void)
{
}
