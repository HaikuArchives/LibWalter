// =============================================================================
//
// libwalter ToolbarButton.h
//
// Simple button control for Toolbar
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

// BeOS headers
#include <Bitmap.h>

// libwalter headers
#include "Toolbar.h"
#include "ToolbarButton.h"
#include "ToolbarSupport.h"

// =============================================================================
// ToolbarButton
// =============================================================================

// Constructors and destructors

ToolbarButton::ToolbarButton(const char *name, const char *label,
	BMessage *message, bool switchMode) :
	ToolbarControl(name, message)
{
	_init_object();
	SetLabel(label);
	SetSwitchMode(switchMode);
}

ToolbarButton::ToolbarButton(BMessage *archive) :
	ToolbarControl(archive)
{
	const char *label;
	bool switchMode;
	int value;

	_init_object();

	if (archive->FindString("ToolbarButton::label", &label) == B_OK)
		SetLabel(label);

	if (archive->FindBool("ToolbarButton::switch_mode", &switchMode) == B_OK)
		SetSwitchMode(switchMode);

	if (archive->FindInt32("ToolbarButton::value", (int32*)(&value)) == B_OK)
		SetValue(value);
}	

ToolbarButton::~ToolbarButton()
{
	// Other cleanups
	if (fLabel != NULL) delete[] fLabel;
}

// Private

void ToolbarButton::_calc_size(float &lh, float &lw, float &ph, float &pw)
{
	float w, h;

	// Label size
	w = 0.0;
	h = 0.0;
	if (fLabel != NULL) {
		if (strlen(fLabel) > 0) {
			GetLabelSize(&w, &h);
			if (w >= 0.0 || h >= 0.0) {
				w++;
				h++;
			}
			else {
				w = 0.0;
				h = 0.0;
			}
		}
	}
	lw = w;
	lh = h;

	// Picture size
	GetPictureSize(&w, &h);
	if (w >= 0.0 && h >= 0.0) {
		pw = w + 1.0;
		ph = h + 1.0;
	}
	else {
		pw = 0.0;
		ph = 0.0;
	}

	// We report no picture if we should not draw it, as long as we have at
	// least the label to draw
	if (pw > 0.0 && lw > 0.0 &&
	  ParentToolbar()->PictureSize() == TOOLBAR_PICTURE_NONE) {
		pw = 0.0;
		ph = 0.0;
	}

	// As for picture
	if (lw > 0.0 && pw > 0.0 &&
	  ParentToolbar()->LabelPosition() == TOOLBAR_LABEL_NONE) {
		lw = 0.0;
		lh = 0.0;
	}
}

void ToolbarButton::_init_object(void)
{
	fLabel = NULL;
	fCanvas = NULL;
	fStyle = TOOLBAR_STYLE_FLAT;
	fSwitchMode = false;
	fValue = B_CONTROL_OFF;
}

// Drawing hooks

float ToolbarButton::BorderSize(void)
{
	return (fStyle == TOOLBAR_STYLE_MENU ? 0.0 : 2.0);
}

void ToolbarButton::DrawBackground(BRect updateRect)
{
	// Draw the menu selection background
	if (Enabled() && ParentToolbar()->Enabled() && fMouseOver &&
	  fStyle == TOOLBAR_STYLE_MENU) {
		fCanvas->SetLowColor(ui_color(B_MENU_SELECTION_BACKGROUND_COLOR));
		fCanvas->FillRect(Bounds(), B_SOLID_LOW);
	}
	// Draw the background when value is on
	if (fStyle != TOOLBAR_STYLE_MENU && fValue == B_CONTROL_ON) {
		fCanvas->SetHighColor(255, 255, 255, 128);
		fCanvas->SetDrawingMode(B_OP_ALPHA);
		fCanvas->FillRect(Bounds(), B_SOLID_HIGH);
	}
}

void ToolbarButton::DrawBorder(BRect updateRect)
{
	bool do_draw = false;
	bool pushed = false;

	// Let's see if we have to draw the border, and how ------------------------

	if (fStyle == TOOLBAR_STYLE_FLAT) {
		// Flat toolbar: draw only if the mouse is over us or value is on
		if ((fMouseOver && Enabled() && ParentToolbar()->Enabled()) ||
		  fValue == B_CONTROL_ON) {
			do_draw = true;
			pushed = (fMouseDown || fValue == B_CONTROL_ON);
		}
	}
	else if (fStyle == TOOLBAR_STYLE_MENU) {
		// Menu: just don't do it
	}
	else {
		// 3D or unknown toolbar: draw it, boy
		do_draw = true;
		pushed = (fMouseDown || fValue == B_CONTROL_ON);
	}

	// Draw the border ---------------------------------------------------------

	if (do_draw)
		ToolbarSupport::Draw3DBorder(fCanvas, Bounds(), pushed);
}

void ToolbarButton::DrawLabel(BPoint position, BRect updateRect)
{
	BPoint realPos(position);
	bool pushed = false;
	font_height fe;

	// BeOS draws text at baseline, not at top. One pixel over the baseline,
	// to be precise.
	fCanvas->GetFontHeight(&fe);
	realPos.y += fe.ascent + 1.0;

	// If the button is pushed, draw the label one pixel off
	if (fStyle != TOOLBAR_STYLE_MENU &&
	  ((fMouseOver && fMouseDown) || fValue == B_CONTROL_ON))
		pushed = true;
	if (pushed) {
		realPos.x++;
		realPos.y++;
	}

	// Draw the text
	fCanvas->SetDrawingMode(B_OP_ALPHA);
	if (Enabled() && ParentToolbar()->Enabled()) {
		if (fStyle == TOOLBAR_STYLE_MENU && fMouseOver && Enabled() &&
		  ParentToolbar()->Enabled())
			fCanvas->SetHighColor(ui_color(B_MENU_SELECTED_ITEM_TEXT_COLOR));
		fCanvas->DrawString(fLabel, realPos);
	}
	else {
		fCanvas->SetHighColor(tint_color(fCanvas->LowColor(),
			B_LIGHTEN_MAX_TINT));
		fCanvas->DrawString(fLabel, BPoint(realPos.x + 1.0, realPos.y + 1.0));
		fCanvas->SetHighColor(tint_color(fCanvas->LowColor(), B_DARKEN_2_TINT));
		fCanvas->DrawString(fLabel, realPos);
	}
}

void ToolbarButton::DrawPicture(BPoint position, BRect updateRect)
{
	// Draw the menu check mark
	if (fStyle == TOOLBAR_STYLE_MENU && fValue == B_CONTROL_ON) {
		float picSize, width, x;
		BBitmap *mark;

		mark = GetMenuCheckMark();

		// Real X position
		width = mark->Bounds().Width() + 1.0;
		picSize = (float)(ParentToolbar()->PictureSize());
		x = (width < picSize ?
			position.x + floor((picSize - width) / 2.0) :
			position.x);
		x += 3.0;

		// Draw the check mark
		fCanvas->SetDrawingMode(B_OP_OVER);
		fCanvas->DrawBitmap(mark, BPoint(x, position.y));

		delete mark;
	}
}

void ToolbarButton::GetLabelSize(float *width, float *height)
{
	// We reserve one more pixels in each direction for disabled effect, and
	// one pixel for pushed (but only of we're not in menu style)
	float w = 1.0, h = 1.0;
	font_height fe;
	ParentToolbar()->GetFontHeight(&fe);
	w += ceil(ParentToolbar()->StringWidth(fLabel));
	h += ceil(fe.ascent + fe.descent);
	if (fStyle != TOOLBAR_STYLE_MENU) {
		w++;
		h++;
	}
	if (width != NULL) *width = w - 1.0;
	if (height != NULL) *height = h - 1.0;
}

void ToolbarButton::GetPictureSize(float *width, float *height)
{
	float w = 0.0, h = 0.0;
	// If style is menu, we have to make sure we have room for the check
	// mark, and follows the same rules of ToolbarBitmapButton to match style
	if (fStyle == TOOLBAR_STYLE_MENU) {
		BBitmap *mark;
		float picSize;
		mark = GetMenuCheckMark();
		w = mark->Bounds().Width() + 1.0;
		h = mark->Bounds().Height() + 1.0;
		delete mark;
		picSize = ParentToolbar()->PictureSize();
		if (w < picSize)
			w = picSize;
		w += 6.0;
	}
	if (width != NULL) *width = w - 1.0;
	if (height != NULL) *height = h - 1.0;
}

float ToolbarButton::Padding(void)
{
	return 2.0;
}

// BArchivable hooks

status_t ToolbarButton::Archive(BMessage *archive, bool deep) const
{
	status_t status;

	status = ToolbarControl::Archive(archive, deep);

	if (status == B_OK && fLabel != NULL) {
		if (strlen(fLabel) > 0)
			status = archive->AddString("ToolbarButton::label", fLabel);
	}

	if (status == B_OK)
		status = archive->AddBool("ToolbarButton::switch_mode", fSwitchMode);

	if (status == B_OK)
		status = archive->AddInt32("ToolbarButton::value", (int32)fValue);

	return status;
}

BArchivable * ToolbarButton::Instantiate(BMessage *archive)
{
	return (validate_instantiation(archive, "ToolbarButton") ?
		new ToolbarButton(archive) : NULL);
}

// ToolbarControl hooks

void ToolbarButton::Draw(BView *canvas, BRect updateRect)
{
	if (ParentToolbar() == NULL) return;

	float lh, lw, ph, pw, border, padding;
	BPoint lp(0.0, 0.0), pp(0.0, 0.0);

	// Prepare infos for protected drawing hooks
	fCanvas = canvas;
	fStyle = ParentToolbar()->Style();

	// Border and padding. We do it after setting infos because BorderSize()
	// and Padding() may need them
	border = BorderSize();
	padding = Padding();

	// Draw the background
	fCanvas->PushState();
	DrawBackground(updateRect);
	fCanvas->PopState();

	// Draw the border
	fCanvas->PushState();
	DrawBorder(updateRect);
	fCanvas->PopState();

	// Label and picture size and position
	_calc_size(lh, lw, ph, pw);
	if (lw == 0.0 && pw == 0.0) return;
	if (lw > 0.0 && pw > 0.0) {							// Picture and label
		float tw, th, ax, ay;
		if (ParentToolbar()->LabelPosition() == TOOLBAR_LABEL_SIDE) {
			tw = pw + padding + lw;
			th = (lh > ph ? lh : ph);
			ax = (fStyle == TOOLBAR_STYLE_MENU ? border + padding :
				(Bounds().Width() + 1.0 - tw) / 2.0);
			ay = (Bounds().Height() + 1.0 - th) / 2.0;
			lp.x = ax + pw + padding;
			lp.y = ay;
			pp.x = ax;
			pp.y = ay;
			if (ph > lh) lp.y += (ph - lh) / 2.0;
			if (lh > ph) pp.y += (lh - ph) / 2.0;
		}
		else {
			tw = (lw > pw ? lw : pw);
			th = ph + padding + lh;
			ax = (fStyle == TOOLBAR_STYLE_MENU ? border + padding :
				(Bounds().Width() + 1.0 - tw) / 2.0);
			ay = (Bounds().Height() + 1.0 - th) / 2.0;
			lp.x = ax;
			lp.y = ay + ph + padding;
			pp.x = ax;
			pp.y = ay;
			if (pw > lw) lp.x += (pw - lw) / 2.0;
			if (lw > pw) pp.x += (lw - pw) / 2.0;
		}
	}
	if (lw == 0.0 && pw > 0.0) {						// Picture only
		pp.x = (fStyle == TOOLBAR_STYLE_MENU ? border + padding :
			(Bounds().Width() + 1.0 - pw) / 2.0);
		pp.y = (Bounds().Height() + 1.0 - ph) / 2.0;
	}
	if (lw > 0.0 && pw == 0.0) {						// Label only
		lp.x = (fStyle == TOOLBAR_STYLE_MENU ? border + padding :
			(Bounds().Width() + 1.0 - lw) / 2.0);
		lp.y = (Bounds().Height() + 1.0 - lh) / 2.0;
	}

	// Adjust position on origin
	if (lw > 0.0) {
		lp.x += Bounds().left;
		lp.y += Bounds().top;
	}
	if (pw > 0.0) {
		pp.x += Bounds().left;
		pp.y += Bounds().top;
	}

	// Draw the label
	// TODO check updateRect and set clipping region
	if (lw > 0.0) {
		fCanvas->PushState();
		DrawLabel(lp, updateRect);
		fCanvas->PopState();
	}

	// Draw the picture
	// TODO check updateRect and set clipping region
	if (pw > 0.0) {
		fCanvas->PushState();
		DrawPicture(pp, updateRect);
		fCanvas->PopState();
	}

	// Debug stuff
	/*canvas->PushState();
	canvas->SetHighColor(255, 0, 0);
	if (lw > 0)
		canvas->StrokeRect(BRect(lp, BPoint(lp.x + lw - 1, lp.y + lh - 1)));
	if (pw > 0)
		canvas->StrokeRect(BRect(pp, BPoint(pp.x + pw - 1, pp.y + ph - 1)));
	canvas->PopState();*/
}

void ToolbarButton::GetPreferredSize(float *width, float *height)
{
	// We must be attached to a toolbar
	if (ParentToolbar() == NULL) {
		if (width != NULL) *width = -1.0;
		if (height != NULL) *height = -1.0;
		return;
	}

	float border, padding, h = 0.0, w = 0.0, lh, lw, ph, pw;

	// Set infos for protected drawing hooks
	fCanvas = ParentToolbar();
	fStyle = ParentToolbar()->Style();

	// Sizes
	border = BorderSize();
	padding = Padding();
	_calc_size(lh, lw, ph, pw);

	// Size for each possible condition	

	if (lw > 0.0 && pw > 0.0) {					// Picture and label
		switch (ParentToolbar()->LabelPosition()) {
			case TOOLBAR_LABEL_SIDE:
				w = lw + pw + padding;
				h = (lh > ph ? lh : ph);
				break;
			case TOOLBAR_LABEL_BOTTOM:
				w = (lw > pw ? lw : pw);
				h = lh + ph + padding;
				break;
			default:
				// TOOLBAR_LABEL_NONE can't be, because _calc_size()
				// sets lw and lh to 0.0 in this case, or there's no picture.
				break;
		}
	}

	if (lw == 0.0 && pw > 0.0) {				// Picture only
		w = pw;
		h = ph;
	}

	if (lw > 0.0 && pw == 0.0) {				// Label only
		w = lw;
		h = lh;
	}

	if (lw == 0.0 && pw == 0.0) {				// Nothing???
		w = 0.0;
		h = 0.0;
	}

	// Don't forget about border and padding!
	w += border * 2.0 + padding * 2.0;
	h += border * 2.0 + padding * 2.0;

	if (width != NULL) *width = w - 1.0;
	if (height != NULL) *height = h - 1.0;
}

void ToolbarButton::MouseUp(BPoint point)
{
	fMouseDown = false;
	if (fMouseOver && Enabled()) {
		if (fSwitchMode)
			fValue = (fValue == B_CONTROL_ON ? B_CONTROL_OFF : B_CONTROL_ON);
		Invoke();
	}
	Invalidate();
}

// Other methods

BBitmap * ToolbarButton::GetMenuCheckMark(void)
{
	static const unsigned char kCheckMarkBitmapData[] = {
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0C, 0x0C, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0C, 0x0C, 0x17, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0C, 0x0C, 0x17, 0x17, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0C, 0x0C, 0x17, 0xFF, 0xFF,
		0xFF, 0x0C, 0x0C, 0xFF, 0xFF, 0xFF, 0x0C, 0x0C, 0x17, 0x17, 0xFF, 0xFF,
		0xFF, 0x0C, 0x0C, 0xFF, 0xFF, 0xFF, 0x0C, 0x0C, 0x17, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0x0C, 0x0C, 0xFF, 0x0C, 0x0C, 0x17, 0x17, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0x0C, 0x0C, 0xFF, 0x0C, 0x0C, 0x17, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0x0C, 0x0C, 0x0C, 0x17, 0x17, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0x0C, 0x0C, 0x0C, 0x17, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0x0C, 0x17, 0x17, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
	};
	BBitmap *ret = new BBitmap(BRect(0.0, 0.0, 11.0, 11.0), B_CMAP8);
	memcpy(ret->Bits(), kCheckMarkBitmapData, 144);
	return ret;
}

const char * ToolbarButton::Label(void)
{
	return fLabel;
}

void ToolbarButton::SetLabel(const char *label)
{
	if (fLabel == NULL && label == NULL) return;
	if (fLabel != NULL && label != NULL)
		if (strcmp(fLabel, label) == 0) return;
	if (fLabel != NULL) {
		delete[] fLabel;
		fLabel = NULL;
	}
	if (label != NULL) {
		if (strlen(label) > 0) {
			fLabel = strdup(label);
		}
	}
	Invalidate();
}

void ToolbarButton::SetValue(int32 value)
{
	if ((value != B_CONTROL_ON && value != B_CONTROL_OFF) || (fValue == value))
		return;
	fValue = value;
	Invalidate();
}

void ToolbarButton::SetSwitchMode(bool switchMode)
{
	if (fSwitchMode == switchMode) return;
	fSwitchMode = switchMode;
	Invalidate();
}

bool ToolbarButton::SwitchMode(void)
{
	return fSwitchMode;
}

int32 ToolbarButton::Value(void)
{
	return fValue;
}
