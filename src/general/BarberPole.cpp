/*
 * Copyright 1998 dirk116, dirk116@wallace.free.de
 * Copyright 2003 Michael Pfeiffer
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include "BarberPole.h"
#include <Region.h>

BBarberPole::BBarberPole(BRect pRect, const char *pName, uint32 resizingMode, uint32 flags, int pDirection)
		: BView(pRect, pName, resizingMode, flags | B_WILL_DRAW | B_PULSE_NEEDED) {
		
	SetViewColor(B_TRANSPARENT_COLOR);
	fStripes.data[0] = 0x0f;
	fStripes.data[1] = 0x1e;
	fStripes.data[2] = 0x3c;
	fStripes.data[3] = 0x78;
	fStripes.data[4] = 0xf0;
	fStripes.data[5] = 0xe1;
	fStripes.data[6] = 0xc3;
	fStripes.data[7] = 0x87;	
	fDirection = pDirection;
};


#if BB_LOOK_3D
void BBarberPole::SetColors(rgb_color* colors, rgb_color c) {
	int alpha = 255/BB_NUM_SHADES;
	for (int i = 0; i < BB_NUM_SHADES; i ++) {
		colors[i].red = c.red * alpha / 255;
		colors[i].green = c.green * alpha / 255;
		colors[i].blue = c.blue * alpha / 255;
		colors[i].alpha = 255;
		alpha += 255/BB_NUM_SHADES;
	}
}
void BBarberPole::SetLowColor(rgb_color c) {
	SetColors(fHighColors, c);
}

void BBarberPole::SetHighColor(rgb_color c) {
	SetColors(fLowColors, c);
}
#endif

void BBarberPole::Draw(BRect r) {
#if BB_LOOK_3D
	BRect b(Bounds());
	BRect b1;
	for (int i = 0; i < BB_NUM_SHADES; i ++) {
		BRegion r(b);
		if (i != (BB_NUM_SHADES-1)) {
			b1 = b.InsetByCopy(1, 1);
			r.Exclude(b1);
		}
		ConstrainClippingRegion(&r);
		BView::SetHighColor(fHighColors[i]); BView::SetLowColor(fLowColors[i]);
		FillRect(b, fStripes);
		b.InsetBy(1, 1);
	}
#else
	FillRect(r, fStripes);
#endif
}

void BBarberPole::Pulse() {
	if(fDirection == FROM_RIGHT_TO_LEFT){
		uchar tmp = fStripes.data[0];
		for (int j = 0; j < 7; ++j) {
  			fStripes.data[j] = fStripes.data[j+1];
		}
		fStripes.data[7] = tmp;
	} else {
		uchar tmp = fStripes.data[7];
		for (int j = 7; j > 0; --j) {
  			fStripes.data[j] = fStripes.data[j-1];
		}
		fStripes.data[0] = tmp;
	}
	Invalidate();
}

