// =============================================================================
//
// libwalter ToolbarSeparator.cpp
//
// Separator control for AKToolbar
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

// libwalter headers
#include "Toolbar.h"
#include "ToolbarSeparator.h"

// =============================================================================
// ToolbarSeparator
// =============================================================================

/*!
 * \class ToolbarSeparator
 * \brief Separator control for toolbars.
 *
 * The separator looks like a 3D inset line if the toolbar's style is
 * TOOLBAR_STYLE_FLAT or TOOLBAR_STYLE_MENU, or a simple empty space if
 * style is TOOLBAR_STYLE_3D. It is vertical if the toolbar is
 * horizontal, or horizontal if the toolbar is vertical. The empty space size
 * is the double of toolbar's padding.
 */

// Constructors and destructors

/*!
 * \brief Creates a new toolbar separator.
 *
 * @param[in] name Name of the control, passed straight to the AKToolbarControl
 *                 constructor.
 */
ToolbarSeparator::ToolbarSeparator(const char *name) :
	ToolbarControl(name, NULL)
{
}

ToolbarSeparator::ToolbarSeparator(BMessage *archive) :
	ToolbarControl(archive)
{
	// Nothing to do here!
}

ToolbarSeparator::~ToolbarSeparator()
{
}

// BArchivable hooks

status_t ToolbarSeparator::Archive(BMessage *archive, bool deep) const
{
	// We don't have nothing to archive
	return ToolbarControl::Archive(archive, deep);
}

BArchivable * ToolbarSeparator::Instantiate(BMessage *archive)
{
	return (validate_instantiation(archive, "ToolbarSeparator") ?
		new ToolbarSeparator(archive) : NULL);
}

// ToolbarControl hooks

void ToolbarSeparator::Draw(BView *canvas, BRect updateRect)
{
	if (ParentToolbar() == NULL) return;
	switch (ParentToolbar()->Style()) {
		case TOOLBAR_STYLE_3D:
			break;
		case TOOLBAR_STYLE_FLAT:
		case TOOLBAR_STYLE_MENU:
		default: {
			float l, r, t, b, p;
			p = ((ParentToolbar()->Alignment() == TOOLBAR_VERTICAL ?
				Bounds().Height() : Bounds().Width()) + 1) / 2.0 - 1.0;
			l = Bounds().left;
			r = Bounds().right;
			t = Bounds().top;
			b = Bounds().bottom;
			canvas->BeginLineArray(2);
			if (ParentToolbar()->Alignment() == TOOLBAR_VERTICAL) {
				canvas->AddLine(BPoint(l, t + p), BPoint(r, t + p),
					tint_color(canvas->LowColor(), B_DARKEN_2_TINT));
				canvas->AddLine(BPoint(l, t + p + 1.0), BPoint(r, t + p + 1.0),
					tint_color(canvas->LowColor(), B_LIGHTEN_MAX_TINT));
			}
			else {
				canvas->AddLine(BPoint(l + p, t), BPoint(l + p, b),
					tint_color(canvas->LowColor(), B_DARKEN_2_TINT));
				canvas->AddLine(BPoint(l + p + 1.0, t), BPoint(l + p + 1.0, b),
					tint_color(canvas->LowColor(), B_LIGHTEN_MAX_TINT));
			}
			canvas->EndLineArray();
			} break;
	}
}

void ToolbarSeparator::GetPreferredSize(float *width, float *height)
{
	float w, h, s = 0, o = 0;
	if (ParentToolbar() != NULL) {
		switch (ParentToolbar()->Style()) {
			case TOOLBAR_STYLE_3D:
				s = ParentToolbar()->Padding() * 2.0;
				break;
			case TOOLBAR_STYLE_MENU: {
				font_height fe;
				if (ParentToolbar()->LockLooper()) {
					ParentToolbar()->GetFontHeight(&fe);
					ParentToolbar()->UnlockLooper();
				}
				s = fe.ascent + fe.descent + fe.leading;
				o = 2.0;
				} break;
			case TOOLBAR_STYLE_FLAT:
			default:
				s = ParentToolbar()->Padding() * 2.0 + 2.0;
				o = 2.0;
				break;
		}
		if (ParentToolbar()->Alignment() == TOOLBAR_VERTICAL) {
			w = o;
			h = s;
		}
		else {
			w = s;
			h = o;
		}
	}
	if (width != NULL) *width = w;
	if (height != NULL) *height = h;
}
