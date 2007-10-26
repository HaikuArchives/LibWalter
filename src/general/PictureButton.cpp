// ============================================================================
//
// libwalter PictureButton.cpp
//
// Distributed under the terms of the MIT License.
//
// Authors:
//		Gabriele Biffi - http://www.biffuz.it/
//
// Contains portions from Haiku, copyright 2001-2005.
//
// ============================================================================

// Haiku headers
#include <Bitmap.h>
#include <Window.h>

// libwalter headers
#include "PictureButton.h"

// ============================================================================
// Constructors and destructor
// ============================================================================

PictureButton::PictureButton(BRect frame, const char *name, const char *label,
	BBitmap *picture, BDPosition position, BMessage *message,
	uint32 resizingMode, uint32 flags) :
	BButton(frame, name, label, message, resizingMode,
		flags | B_FULL_UPDATE_ON_RESIZE | B_NAVIGABLE | B_WILL_DRAW)
{
	float width = frame.Width(), height = frame.Height();

	fDecorator = new ButtonDecorator(label, picture, position);

	// Make sure the button is large enough
	GetPreferredSize(NULL, NULL);
	ResizeTo((width < fMinWidth ? fMinWidth : width),
		(height < fMinHeight ? fMinHeight : height));
}

PictureButton::PictureButton(BMessage *archive) :
	BButton(archive)
{
	float width, height;
	BMessage message;
	BRect frame;

	// Decorator
	fDecorator = NULL;
	if (archive != NULL) {
		if (archive->FindMessage("PictureButton::decorator",
		  &message) == B_OK) {
			BArchivable *archivable;
			archivable = ButtonDecorator::Instantiate(&message);
			if (archivable != NULL) {
				fDecorator = dynamic_cast<ButtonDecorator*>(archivable);
				if (fDecorator == NULL)
					delete archivable;
			}
		}
	}
	if (fDecorator == NULL)
		fDecorator = new ButtonDecorator(NULL, NULL, BD_POSITION_ABOVE);
	SetLabel(Label());	// Make sure the label is the same for both BButton
						// and ButtonDecorator

	// Make sure the button is large enough
	frame = Frame();
	width = frame.Width();
	height = frame.Height();
	GetPreferredSize(NULL, NULL);
	ResizeTo((width < fMinWidth ? fMinWidth : width),
		(height < fMinHeight ? fMinHeight : height));
}

PictureButton::~PictureButton()
{
	delete fDecorator;
}

// ============================================================================
// Private
// ============================================================================

BRect PictureButton::_draw_default(BRect bounds, bool enabled)
{
	rgb_color low = LowColor();
	rgb_color focusColor = tint_color(low,
		enabled ? (B_DARKEN_1_TINT + B_DARKEN_2_TINT) / 2 :
		(B_NO_TINT + B_DARKEN_1_TINT) / 2);

	SetHighColor(focusColor);

	StrokeRect(bounds, B_SOLID_LOW);
	bounds.InsetBy(1.0, 1.0);
	StrokeRect(bounds);
	bounds.InsetBy(1.0, 1.0);
	StrokeRect(bounds);
	bounds.InsetBy(1.0, 1.0);

	return bounds;
}

void PictureButton::_draw_focus_line(float x, float y, float width,
	bool visible)
{
	if (visible)
		SetHighColor(ui_color(B_KEYBOARD_NAVIGATION_COLOR));
	else
		SetHighColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR),
			B_LIGHTEN_1_TINT));

	// Blue Line
	StrokeLine(BPoint(x, y), BPoint(x + width, y));

	if (visible)		
		SetHighColor(255, 255, 255);
	// White Line
	StrokeLine(BPoint(x, y + 1.0f), BPoint(x + width, y + 1.0f));
}

// ============================================================================
// Static
// ============================================================================

BArchivable * PictureButton::Instantiate(BMessage *archive)
{
	return (validate_instantiation(archive, "PictureButton") ?
		new PictureButton(archive) : NULL);
}

// ============================================================================
// Public
// ============================================================================

status_t PictureButton::Archive(BMessage *archive, bool deep) const
{
	status_t status = BButton::Archive(archive, deep);

	if (status == B_OK) {
		BMessage message;
		status = fDecorator->Archive(&message, deep);
		if (status == B_OK)
			status = archive->AddMessage("PictureButton::decorator", &message);
	}

	return status;
}

ButtonDecorator * PictureButton::Decorator(void)
{
	return fDecorator;
}

void PictureButton::Draw(BRect updateRect)
{
	if (Window() == NULL) return;

	BRect rect, contentRect;
	float width, height;

	// Check that the button is large enough
	rect = Bounds();
	width = rect.Width();
	height = rect.Height();
	rect.right = rect.left + (width < fMinWidth ? fMinWidth : width);
	rect.bottom = rect.top + (height < fMinHeight ? fMinHeight : height);

	// Status
	const bool enabled = IsEnabled();
	const bool pushed = (Value() == B_CONTROL_ON);

	// Default indicator
	if (IsDefault())
		rect = _draw_default(rect, enabled);

	// Content rectangle
	contentRect = BRect(0.0, 0.0, fContentWidth, fContentHeight);
	contentRect.OffsetTo(
		rect.left + floor((rect.Width() - fContentWidth) / 2.0),
		rect.top + floor((rect.Height() - fContentHeight) / 2.0));

	/* speed trick:
	   if the focus changes but the button is not pressed then we can
	   redraw only the focus line, 
	   if the focus changes and the button is pressed invert the internal rect	
	   this block takes care of all the focus changes 	
	*/
	if (IsFocusChanging()) {
		if (pushed) {
			rect.InsetBy(2.0, 2.0);
			InvertRect(rect);
		}
		else 
			_draw_focus_line(contentRect.left, contentRect.bottom + 1.0,
				contentRect.Width(), IsFocus() && Window()->IsActive());
		return;
	}

	// Colors	
	rgb_color panelBgColor = ui_color(B_PANEL_BACKGROUND_COLOR);
	rgb_color buttonBgColor = tint_color(panelBgColor, B_LIGHTEN_1_TINT);	
	rgb_color lightColor;
	rgb_color maxLightColor;	
	rgb_color maxShadowColor = tint_color(panelBgColor, B_DARKEN_MAX_TINT);	

	rgb_color dark1BorderColor;
	rgb_color dark2BorderColor;

	rgb_color bevelColor1;
	rgb_color bevelColor2;
	rgb_color bevelColorRBCorner;

	rgb_color borderBevelShadow;
	rgb_color borderBevelLight;

	if (enabled) {
		lightColor = tint_color(panelBgColor, B_LIGHTEN_2_TINT);
		maxLightColor = tint_color(panelBgColor, B_LIGHTEN_MAX_TINT);	
	
		dark1BorderColor = tint_color(panelBgColor, B_DARKEN_3_TINT);
		dark2BorderColor = tint_color(panelBgColor, B_DARKEN_4_TINT);
	
		bevelColor1 = tint_color(panelBgColor, B_DARKEN_2_TINT);
		bevelColor2 = panelBgColor;

		if (IsDefault()) {
			borderBevelShadow = tint_color(dark1BorderColor,
				(B_NO_TINT + B_DARKEN_1_TINT) / 2);
			borderBevelLight = tint_color(dark1BorderColor, B_LIGHTEN_1_TINT);

			borderBevelLight.red = (borderBevelLight.red + panelBgColor.red) /
				2;
			borderBevelLight.green = (borderBevelLight.green +
				panelBgColor.green) / 2;
			borderBevelLight.blue = (borderBevelLight.blue +
				panelBgColor.blue) / 2;
	
			dark1BorderColor = tint_color(dark1BorderColor, B_DARKEN_3_TINT);
			dark2BorderColor = tint_color(dark1BorderColor, B_DARKEN_4_TINT);

			bevelColorRBCorner = borderBevelShadow;
		}
		else {
			borderBevelShadow = tint_color(panelBgColor,
				(B_NO_TINT + B_DARKEN_1_TINT) / 2);
			borderBevelLight = buttonBgColor;

			bevelColorRBCorner = dark1BorderColor;
		}
	}
	else {
		lightColor = tint_color(panelBgColor, B_LIGHTEN_2_TINT);
		maxLightColor = tint_color(panelBgColor, B_LIGHTEN_1_TINT);	
	
		dark1BorderColor = tint_color(panelBgColor, B_DARKEN_1_TINT);
		dark2BorderColor = tint_color(panelBgColor, B_DARKEN_2_TINT);
	
		bevelColor1 = panelBgColor;
		bevelColor2 = buttonBgColor;

		if (IsDefault()) {
			borderBevelShadow = dark1BorderColor;
			borderBevelLight = panelBgColor;
			dark1BorderColor = tint_color(dark1BorderColor, B_DARKEN_1_TINT);
			dark2BorderColor = tint_color(dark1BorderColor, 1.16);

		}
		else {
			borderBevelShadow = panelBgColor;
			borderBevelLight = panelBgColor;
		}

		bevelColorRBCorner = tint_color(panelBgColor, 1.08);
	}

	BeginLineArray(22);

	// Bevel around external border
	AddLine(BPoint(rect.left, rect.bottom),
			BPoint(rect.left, rect.top), borderBevelShadow);
	AddLine(BPoint(rect.left + 1, rect.top),
			BPoint(rect.right, rect.top), borderBevelShadow);

	AddLine(BPoint(rect.right, rect.top + 1),
			BPoint(rect.right, rect.bottom), borderBevelLight);
	AddLine(BPoint(rect.left + 1, rect.bottom),
			BPoint(rect.right - 1, rect.bottom), borderBevelLight);

	rect.InsetBy(1.0, 1.0);

	// External border
	AddLine(BPoint(rect.left, rect.bottom),
			BPoint(rect.left, rect.top), dark1BorderColor);
	AddLine(BPoint(rect.left + 1, rect.top),
			BPoint(rect.right, rect.top), dark1BorderColor);
	AddLine(BPoint(rect.right, rect.top + 1),
			BPoint(rect.right, rect.bottom), dark2BorderColor);
	AddLine(BPoint(rect.right - 1, rect.bottom),
			BPoint(rect.left + 1, rect.bottom), dark2BorderColor);

	rect.InsetBy(1.0, 1.0);
	
	// Light
	AddLine(BPoint(rect.left, rect.top),
			BPoint(rect.left, rect.top), buttonBgColor);
	AddLine(BPoint(rect.left, rect.top + 1),
			BPoint(rect.left, rect.bottom - 1), lightColor);
	AddLine(BPoint(rect.left, rect.bottom),
			BPoint(rect.left, rect.bottom), bevelColor2);
	AddLine(BPoint(rect.left + 1, rect.top),
			BPoint(rect.right - 1, rect.top), lightColor);	
	AddLine(BPoint(rect.right, rect.top),
			BPoint(rect.right, rect.top), bevelColor2);	
	// Shadow
	AddLine(BPoint(rect.left + 1, rect.bottom),
			BPoint(rect.right - 1, rect.bottom), bevelColor1);
	AddLine(BPoint(rect.right, rect.bottom),
			BPoint(rect.right, rect.bottom), bevelColorRBCorner);
	AddLine(BPoint(rect.right, rect.bottom - 1),
			BPoint(rect.right, rect.top + 1), bevelColor1);
	
	rect.InsetBy(1.0, 1.0);
	
	// Light
	AddLine(BPoint(rect.left, rect.top),
			BPoint(rect.left, rect.bottom - 1), maxLightColor);
	AddLine(BPoint(rect.left, rect.bottom),
			BPoint(rect.left, rect.bottom), buttonBgColor);
	AddLine(BPoint(rect.left + 1, rect.top),
			BPoint(rect.right - 1, rect.top), maxLightColor);	
	AddLine(BPoint(rect.right, rect.top),
			BPoint(rect.right, rect.top), buttonBgColor);	
	// Shadow
	AddLine(BPoint(rect.left + 1, rect.bottom),
			BPoint(rect.right, rect.bottom), bevelColor2);
	AddLine(BPoint(rect.right, rect.bottom - 1),
			BPoint(rect.right, rect.top + 1), bevelColor2);
	
	rect.InsetBy(1.0,1.0);

	EndLineArray();	

	// Fill the content area
	SetHighColor(buttonBgColor);
	FillRect(rect);

	// Draw the content
	SetHighColor(0, 0, 0);
	fDecorator->Draw(this, contentRect, (enabled ? BD_STATUS_NORMAL :
		BD_STATUS_DISABLED));

	// Invert if clicked
	if (enabled && pushed) {
		rect.InsetBy(-2.0, -2.0);
		InvertRect(rect);
	}

	// Focus line
	if (enabled && IsFocus() && Window()->IsActive() && !pushed)
		_draw_focus_line(contentRect.left, contentRect.bottom + 1.0,
			contentRect.Width(), true);
}

void PictureButton::GetPreferredSize(float *width, float *height)
{
	float inset = (IsDefault() ? 26.0 : 20.0);
	fDecorator->GetPreferredSize(this, &fContentWidth, &fContentHeight);
 	fMinWidth = fContentWidth + inset;
 	fMinHeight = fContentHeight + inset;
	if (width != NULL) *width = fMinWidth;
	if (height != NULL) *height = fMinHeight;
}

void PictureButton::MakeDefault(bool flag)
{
	// BButton::MakeDefault acts differently in different versions of BeOS,
	// so we have to resize the button according to Haiku's standard
	bool newDefault, oldDefault;
	BRect frame;
	float inset;

	oldDefault = IsDefault();
	frame = Frame();

	BButton::MakeDefault(flag);

	newDefault = IsDefault();
	if (newDefault != oldDefault) {
		inset = (newDefault ? -3.0 : 3.0);
		frame.InsetBy(inset, inset);
		ResizeTo(frame.Width(), frame.Height());
		MoveTo(frame.LeftTop());
	}

	GetPreferredSize(NULL, NULL);
}

BBitmap * PictureButton::Picture(void)
{
	BBitmap *bitmaps[8];
	fDecorator->GetPicture(bitmaps);
	return bitmaps[0];
}

BDPictureType PictureButton::PictureType(void)
{
	return fDecorator->PictureType();
}

BDPosition PictureButton::Position(void)
{
	return fDecorator->Position();
}

void PictureButton::SetLabel(const char *label)
{
	BButton::SetLabel(label);
	fDecorator->SetLabel(label);
	GetPreferredSize(NULL, NULL);
}

bool PictureButton::SetPicture(BBitmap *picture)
{
	BBitmap	*bitmaps[8];
	float size;
	bool ret;

	fDecorator->DeletePicture();
	if (picture == NULL) {
		GetPreferredSize(NULL, NULL);
		Invalidate();
		return true;
	}

	bitmaps[0] = picture;
	for (int i = 1; i < 8; i++)
		bitmaps[i] = NULL;
	ret = fDecorator->SetPicture(bitmaps);

	if (ret) {
		size = picture->Bounds().Width() + 1.0;
		fDecorator->SetPictureSize(size);
		GetPreferredSize(NULL, NULL);
	}

	Invalidate();
	return ret;
}

void PictureButton::SetPosition(BDPosition position)
{
	fDecorator->SetPosition(position);
	if (fDecorator->NeedsUpdate()) {
		GetPreferredSize(NULL, NULL);
		Invalidate();
	}
}

void PictureButton::SetVisible(BDVisible visible)
{
	fDecorator->SetVisible(visible);
	if (fDecorator->NeedsUpdate()) {
		GetPreferredSize(NULL, NULL);
		Invalidate();
	}
}

BDVisible PictureButton::Visible(void)
{
	return fDecorator->Visible();
}
