/*
 * Copyright 2002-2007, Haiku. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		DarkWyrm (darkwyrm@earthlink.net)
 */
#include "ColorWell.h"
#include <PropertyInfo.h>

static property_info sProperties[] = {
	{ "Red", { B_GET_PROPERTY, 0 }, { B_DIRECT_SPECIFIER, 0 },
		"Returns the red value for the color well.", 0, { B_INT32_TYPE }
	},
	{ "Red", { B_SET_PROPERTY, 0 }, { B_DIRECT_SPECIFIER, 0},
		"Sets the red value for the color well.", 0, { B_INT32_TYPE }
	},
	
	{ "Green", { B_GET_PROPERTY, 0 }, { B_DIRECT_SPECIFIER, 0 },
		"Returns the green value for the color well.", 0, { B_INT32_TYPE }
	},
	{ "Green", { B_SET_PROPERTY, 0 }, { B_DIRECT_SPECIFIER, 0},
		"Sets the green value for the color well.", 0, { B_INT32_TYPE }
	},
	
	{ "Blue", { B_GET_PROPERTY, 0 }, { B_DIRECT_SPECIFIER, 0 },
		"Returns the blue value for the color well.", 0, { B_INT32_TYPE }
	},
	{ "Blue", { B_SET_PROPERTY, 0 }, { B_DIRECT_SPECIFIER, 0},
		"Sets the blue value for the color well.", 0, { B_INT32_TYPE }
	},
};

ColorWell::ColorWell(BRect frame, const char *name, BMessage *message,
	int32 resize, int32 flags, int32 style)
	: BControl(frame,name,NULL,message, resize, flags | B_SUBPIXEL_PRECISE)
{
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	SetLowColor(0,0,0);
	
	fDisabledColor.red = 190;
	fDisabledColor.green = 190;
	fDisabledColor.blue = 190;
	fDisabledColor.alpha = 255;
}


ColorWell::ColorWell(BMessage *data)
 : BControl(data)
{
	// This will update fColor
	SetValue(Value());
}


ColorWell::~ColorWell(void)
{
}


BArchivable *
ColorWell::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "ColorWell"))
		return new ColorWell(data);

	return NULL;
}


status_t
ColorWell::Archive(BMessage *data, bool deep = true) const
{
	status_t status = BControl::Archive(data, deep);
	data->AddString("class","ColorWell");
	
	return status;
}

		
status_t
ColorWell::GetSupportedSuites(BMessage *msg)
{
	msg->AddString("suites","suite/vnd.DW-colorwell");
	
	BPropertyInfo prop_info(sProperties);
	msg->AddFlat("messages",&prop_info);
	return BView::GetSupportedSuites(msg);
}


BHandler *
ColorWell::ResolveSpecifier(BMessage *msg, int32 index, BMessage *specifier,
							int32 form, const char *property)
{
	return BControl::ResolveSpecifier(msg, index, specifier, form, property);
}


void
ColorWell::SetValue(int32 value)
{
	BControl::SetValue(value);
	
	fColor.red = (value & 0xFF000000) >> 24;
	fColor.green = (value & 0x00FF0000) >> 16;
	fColor.blue = (value & 0x0000FF00) >> 8;
	
	SetHighColor(fColor);
	Draw(Bounds());
	Invalidate();
}

void
ColorWell::SetValue(const rgb_color &col)
{
	fColor = col;
	
	SetHighColor(col);
	Draw(Bounds());
	Invalidate();
}


void
ColorWell::SetValue(const uint8 &r,const uint8 &g, const uint8 &b)
{
	fColor.red = r;
	fColor.green = g;
	fColor.blue = b;
	
	SetHighColor(r,g,b);
	Draw(Bounds());
	Invoke();
}


void
ColorWell::SetStyle(const int32 &style)
{
	if (style != fStyle) {
		fStyle = style;
		Invalidate();
	}
}


int32
ColorWell::Style(void) const
{
	return fStyle;
}


void
ColorWell::MessageReceived(BMessage *msg)
{
	// If we received a dropped message, try to see if it has color data in it
	if (msg->WasDropped()) {
		
		uint8 *data;
		ssize_t size;
		
		// Note that the pointer is not deleted because the BMessage retains
		// ownership of the data
		if (msg->FindData("RGBColor",B_RGB_COLOR_TYPE,
							(const void**)&data,&size) == B_OK) {
			fColor = *((rgb_color*)data);
			SetHighColor(fColor);
		}
	}

	BView::MessageReceived(msg);
}


void
ColorWell::Draw(BRect update)
{
	if (fStyle == COLORWELL_SQUARE_WELL)
		DrawSquare();
	else
		DrawRound();
}


rgb_color
ColorWell::ValueAsColor(void) const
{
	return fColor;
}


void
ColorWell::DrawRound(void)
{
	BRect r(Bounds());
	r.InsetBy(1,1);
	
	if (IsEnabled()) {
		SetHighColor(fColor);
		FillEllipse(Bounds());
		
		SetPenSize(2.0);
		SetHighColor(tint_color(fColor,B_DARKEN_1_TINT));
		StrokeArc(r, 45, 180);
		
		SetHighColor(tint_color(fColor,B_LIGHTEN_1_TINT));
		StrokeArc(r, 225, 180);
		SetPenSize(1.0);
		
		SetHighColor(0,0,0);
		StrokeEllipse(Bounds());
		
		SetHighColor(0,0,0);
		StrokeArc(Bounds(), 45, 180);
		
		SetHighColor(255,255,255);
		StrokeArc(Bounds(), 225, 180);
		
	} else {
		SetHighColor(fDisabledColor);
		FillEllipse(Bounds());
		
		SetPenSize(2.0);
		SetHighColor(tint_color(fDisabledColor,B_DARKEN_1_TINT));
		StrokeArc(r, 45, 180);
		
		SetHighColor(tint_color(fDisabledColor,B_LIGHTEN_1_TINT));
		StrokeArc(r, 225, 180);
		SetPenSize(1.0);
		
		SetHighColor(0,0,0);
		StrokeEllipse(Bounds());
		
		SetHighColor(120,120,120);
		StrokeArc(Bounds(), 45, 180);
		
		SetHighColor(160,160,160);
		StrokeArc(Bounds(), 225, 180);
	}
}


void
ColorWell::DrawSquare(void)
{
	// This code adapted from Haiku's BButton::Draw
	const rgb_color fillColor = IsEnabled() ? fColor : fDisabledColor;
	const rgb_color highlightHigh = tint_color(fillColor, IsEnabled() ?
														B_LIGHTEN_2_TINT :
														B_LIGHTEN_1_TINT);
	const rgb_color highlightLow = tint_color(fillColor, B_LIGHTEN_1_TINT);
	const rgb_color shadowHigh = tint_color(fillColor, B_DARKEN_1_TINT);
	const rgb_color shadowLow = tint_color(fillColor, IsEnabled() ?
													 B_DARKEN_2_TINT :
													 B_DARKEN_1_TINT);
	const rgb_color panelColor = ui_color(B_PANEL_BACKGROUND_COLOR);
	
	SetHighColor(fillColor);
	FillRect(Bounds());
	
	BRect rect(Bounds());
	
	// external border
	SetHighColor(0,0,0);
	StrokeRect(rect);
		
	BeginLineArray(14);

	// Corners
	AddLine(rect.LeftTop(), rect.LeftTop(), panelColor);
	AddLine(rect.LeftBottom(), rect.LeftBottom(), panelColor);
	AddLine(rect.RightTop(), rect.RightTop(), panelColor);
	AddLine(rect.RightBottom(), rect.RightBottom(), panelColor);

	rect.InsetBy(1.0f,1.0f);
	
	// Shadow
	AddLine(rect.LeftBottom(), rect.RightBottom(), highlightHigh);
	AddLine(rect.RightBottom(), rect.RightTop(), highlightHigh);
	
	// Light
	AddLine(rect.LeftTop(), rect.LeftBottom(),shadowLow);
	AddLine(rect.LeftTop(), rect.RightTop(), shadowLow);	
	
	rect.InsetBy(1.0f, 1.0f);
	
	// Shadow
	AddLine(rect.LeftBottom(), rect.RightBottom(), highlightLow);
	AddLine(rect.RightBottom(), rect.RightTop(), highlightLow);
	
	// Light
	AddLine(rect.LeftTop(), rect.LeftBottom(),shadowHigh);
	AddLine(rect.LeftTop(), rect.RightTop(), shadowHigh);	
	
	EndLineArray();	
}

