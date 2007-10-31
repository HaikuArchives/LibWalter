// =============================================================================
//
// libwalter ToolbarItem.cpp
//
// Base class for WToolbar items
//
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
 * \class WToolbarItem
 * \brief Base class for toolbar items
 *
 * WToolbarItem provides all the basic facilities to create a full featured
 * toolbar item. Everything you want to place in a toolbar must be a descendant
 * of this class; however, if you want a control that your user can interact
 * with, you'll probably want to subclass WToolbarItem. This concept is
 * similar to BView and BControl.
 *
 * libwalter supplies several widely used items and controls, such as
 * WToolbarSeparator and WToolbarButton, and many more.
 *
 * \par How to create your own item
 * Creating your own toolbar item by subclassing WToolbarItem is not very
 * different than subclassing BView. The most obvious methods you have to
 * overload are Draw() and GetPreferredSize().
 *
 * Inside Draw(), you can determine the visual aspect of your item by inspecting
 * the protected properties fMouseDown and fMouseOver. They are set by the
 * default implementations of MouseDown(), MouseUp() and MouseOver(); take care
 * of these properties if you overload these methods. An easy solution is to
 * call the default implementations inside your own.
 */

// libwalter headers
#include "Toolbar.h"
#include "ToolbarItem.h"

// =============================================================================
// WToolbarItem
// =============================================================================

// Constructors and destructors

WToolbarItem::WToolbarItem(const char *name) :
	BHandler(name)
{
	_init_object();
	SetName(name);
}

WToolbarItem::WToolbarItem(BMessage *archive) :
	BHandler(archive)
{
	bool enabled, visible;
	BMessage message;
	int line;

	_init_object();

	if (archive->FindInt32("WToolbarItem::line", (int32*)(&line)) == B_OK)
		fLine = line;

	if (archive->FindBool("WToolbarItem::visible", &visible) == B_OK)
		SetVisible(visible);
}

WToolbarItem::~WToolbarItem()
{
	if (fToolbar != NULL)
		fToolbar->RemoveItem(this);
}

// Private

void WToolbarItem::_init_object(void)
{
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

status_t WToolbarItem::Archive(BMessage *archive, bool deep) const
{
	status_t status;

	// Ancestor
	status = BHandler::Archive(archive, deep);

	// Properties

	if (status == B_OK)
		status = archive->AddInt32("WToolbarItem::line", (int32)fLine);

	if (status == B_OK)
		status = archive->AddBool("WToolbarItem::visible", fVisible);

	return status;
}

BArchivable * WToolbarItem::Instantiate(BMessage *archive)
{
	return (validate_instantiation(archive, "WToolbarItem") ?
		new WToolbarItem(archive) : NULL);
}

// Public

void WToolbarItem::AttachedToToolbar(void)
{
}

BRect WToolbarItem::Bounds(void)
{
	return fFrame.OffsetToCopy(0.0, 0.0);
}

void WToolbarItem::DetachedFromToolbar(void)
{
}

void WToolbarItem::Draw(BView *canvas, BRect update_rect)
{
	// The basic class doesn't draw anything
}

BRect WToolbarItem::Frame(void)
{
	return fFrame;
}

void WToolbarItem::GetPreferredSize(float *width, float *height)
{
	if (width != NULL) *width = -1.0;
	if (height != NULL) *height = -1.0;
}

int WToolbarItem::Index(void)
{
	return (fToolbar == NULL ? -1 : fToolbar->IndexOf(this));
}

void WToolbarItem::Invalidate(void)
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
		fToolbar->DrawItem(this);
}

int WToolbarItem::Line(void)
{
	return (fToolbar == NULL ? -1 : fLine);
}

void WToolbarItem::MouseDown(BPoint point)
{
	if (fMouseOver)
		fMouseDown = true;
	Invalidate();
}

void WToolbarItem::MouseMoved(BPoint point, uint32 transit,
	const BMessage *message)
{
	bool old_mouse_over = fMouseOver;
	fMouseOver = (transit == B_ENTERED_VIEW || transit == B_INSIDE_VIEW);
	if (fMouseOver != old_mouse_over)
		Invalidate();
}

void WToolbarItem::MouseUp(BPoint point)
{
	fMouseDown = false;
	Invalidate();
}

int WToolbarItem::Position(void)
{
	if (fToolbar == NULL) return -1;
	int pos = 0;
	while (fToolbar->ItemAt(fLine, pos) != this)
		pos++;
	return pos;
}

void WToolbarItem::SetVisible(bool visible)
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

WToolbar * WToolbarItem::Toolbar(void)
{
	return fToolbar;
}

void WToolbarItem::Update(void)
{
}

bool WToolbarItem::Visible(void)
{
	return fVisible;
}
