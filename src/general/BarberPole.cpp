/*
 * Copyright 1998 dirk116, dirk116@wallace.free.de
 * Copyright 2003 Michael Pfeiffer
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include "BarberPole.h"
#include <Region.h>

BarberPole::BarberPole(BRect frame, const char *name, uint32 resizingMode, uint32 flags,
						int pDirection)
 :	BView(frame, name, resizingMode, flags | B_WILL_DRAW | B_PULSE_NEEDED),
	fDirection(pDirection),
 	fStyle(BARBERPOLE_FANCY)
{
	SetViewColor(B_TRANSPARENT_COLOR);
	fStripes.data[0] = 0x0f;
	fStripes.data[1] = 0x1e;
	fStripes.data[2] = 0x3c;
	fStripes.data[3] = 0x78;
	fStripes.data[4] = 0xf0;
	fStripes.data[5] = 0xe1;
	fStripes.data[6] = 0xc3;
	fStripes.data[7] = 0x87;
	
	const rgb_color highColor = { 0, 0, 255, 255 };
	SetHighColor(highColor);
	const rgb_color lowColor = { 255, 255, 255, 255 };
	SetLowColor(lowColor);
};


void
BarberPole::SetLowColor(const rgb_color &c)
{
	SetColors(fHighColors, c);
}


void
BarberPole::SetHighColor(const rgb_color &c)
{
	SetColors(fLowColors, c);
}


void
BarberPole::Draw(BRect r)
{
	if (fStyle == BARBERPOLE_FANCY) {	
		BRect b(Bounds());
		BRect b1;
		for (int i = 0; i < 4; i ++) {
			BRegion r(b);
			if (i != (4-1)) {
				b1 = b.InsetByCopy(1, 1);
				r.Exclude(b1);
			}
			ConstrainClippingRegion(&r);
			BView::SetHighColor(fHighColors[i]); BView::SetLowColor(fLowColors[i]);
			FillRect(b, fStripes);
			b.InsetBy(1, 1);
		}
	} else {
		BView::SetHighColor(fHighColors[4 - 1]);
		BView::SetLowColor(fLowColors[4 - 1]);
		FillRect(r, fStripes);
	}
}


void
BarberPole::Pulse(void)
{
	if (fDirection == BARBERPOLE_FROM_RIGHT) {
		uchar tmp = fStripes.data[0];
		for (int j = 0; j < 7; ++j)
  			fStripes.data[j] = fStripes.data[j+1];
		fStripes.data[7] = tmp;
	} else {
		uchar tmp = fStripes.data[7];
		for (int j = 7; j > 0; --j)
  			fStripes.data[j] = fStripes.data[j-1];
		fStripes.data[0] = tmp;
	}
	Invalidate();
}


void
BarberPole::SetStyle(int style)
{
	fStyle = style;
}


int
BarberPole::Style(void) const
{
	return fStyle;
}


void
BarberPole::SetColors(rgb_color *colors, const rgb_color &c)
{
	int alpha = 255 / 4;
	for (int i = 0; i < 4; i++) {
		colors[i].red = c.red * alpha / 255;
		colors[i].green = c.green * alpha / 255;
		colors[i].blue = c.blue * alpha / 255;
		colors[i].alpha = 255;
		alpha += 255 / 4;
	}
}
