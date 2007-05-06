// =============================================================================
//
// libwalter WToolbarSupport.cpp
//
// Miscellaneous functions for WToolbar and related classes
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
 * \class WToolbarSupport
 * \brief Miscellaneous support methods
 */

// Standard C headers
#include <stdlib.h>
#include <string.h>

// BeOS headers
#include <Application.h>
#include <Bitmap.h>
#include <Directory.h>
#include <Entry.h>
#include <File.h>
#include <Menu.h>
#include <MenuItem.h>
#include <Path.h>
#include <Roster.h>
#include <Screen.h>
#include <TextView.h>

// libwalter headers
#include "WToolbarSupport.h"

// =============================================================================
// WToolbarSupport
// =============================================================================

inline uint8 __Yc(uint8 red, uint8 green, uint8 blue)
{
	float R, G, B, Y;
	R = (float)(red) / 255.0;
	G = (float)(green) / 255.0;
	B = (float)(blue) / 255.0;
	Y = (R - G) * 0.299 + G + (B - G) * 0.114;
	return (uint8)(Y * 255.0);
}

/*!
 * \brief Turn a bitmap in grayscale
 *
 * The output bitmap will be of the same size and will use the same color space
 * of the original. Currently supports only B_RGBA32 and B_CMP8 color spaces.
 * Transparency is maintained.
 *
 * \todo Support more color spaces
 *
 * @param[in] bitmap The bitmap you want to turn in grayscale
 *
 * \return If bitmap is  \c NULL or isn't valid, returns  \c NULL. If the
 *         bitmap's color space is not supported, returns a plain copy of it. If
 *         color space is supported, returns a new bitmap. It's up to you to
 *         delete the bitmap when you are done with it.
 */
BBitmap * WToolbarSupport::GrayscaleBitmap(BBitmap *bitmap)
{
	if (bitmap == NULL) return NULL;
	if (!bitmap->IsValid()) return NULL;

	// To obtain the disabled bitmap, we turn the original bitmap in
	// grayscale, converting each pixel from RGB to YCbCr and taking only
	// the Y component (luminance).

	int32 x, y, width, height, offset, row_length;
	unsigned char *i_bits, *o_bits;
	BBitmap *ret;
	uint8 Yc;

	ret = new BBitmap(bitmap->Bounds(), bitmap->ColorSpace());
	i_bits = (unsigned char*)bitmap->Bits();
	o_bits = (unsigned char*)ret->Bits();
	height = bitmap->Bounds().IntegerHeight() + 1;
	width = bitmap->Bounds().IntegerWidth() + 1;
	row_length = bitmap->BytesPerRow();

	switch (bitmap->ColorSpace()) {

		// B_CMAP8: each pixel (one byte) is the index of a color in the
		// system's color table. The table can be obtained from a BScreen
		// object.
		case B_CMAP8: {
			const rgb_color *color;
			const color_map *map;
			BScreen screen;
			map = screen.ColorMap();
			for (y = 0; y < height; y++) {
				for (x = 0; x < width; x++) {
					offset = y * row_length + x;
					if (i_bits[offset] == B_TRANSPARENT_8_BIT)
						o_bits[offset] = B_TRANSPARENT_8_BIT;
					else {
						color = map->color_list + i_bits[offset];
						Yc = __Yc(color->red, color->green, color->blue);
						o_bits[offset] = screen.IndexForColor(Yc, Yc, Yc);
					}
				}
			}
			} break;

		case B_RGBA32:
			for (y = 0; y < height; y++) {
				for (x = 0; x < width; x++) {
					offset = y * row_length + x * 4;
					Yc = __Yc(i_bits[offset], i_bits[offset + 1],
						i_bits[offset + 2]);
					o_bits[offset] = Yc;
					o_bits[offset + 1] = Yc;
					o_bits[offset + 2] = Yc;
					o_bits[offset + 3] = i_bits[offset + 3];
				}
			}
			break;

		default:
			memcpy(ret->Bits(), bitmap->Bits(), bitmap->BitsLength());
			break;
	}

	return ret;
}

/*!
 * \brief Draw a 3D border
 *
 * The border is drawn using the view's low color as a base.
 *
 * @param[in] view The target view
 * @param[in] rect The rectangle wich will contain the border
 * @param[in] inset Leave \c false if you want a raised border, \c true for an
 *                  inset border
 */
void WToolbarSupport::Draw3DBorder(BView *view, BRect rect, bool inset)
{
	if (view == NULL || !(rect.IsValid())) return;

	rgb_color lighter, light, dark, shadow;
	float l, t, r, b;

	view->PushState();

	lighter = tint_color(view->LowColor(), B_DARKEN_2_TINT);
	light = tint_color(view->LowColor(), B_LIGHTEN_MAX_TINT);
	dark = tint_color(view->LowColor(), B_DARKEN_1_TINT);
	shadow = tint_color(view->LowColor(), B_DARKEN_3_TINT);

	l = rect.left;
	t = rect.top;
	r = rect.right;
	b = rect.bottom;

	view->BeginLineArray(8);
	if (inset) {
		view->AddLine(BPoint(r, t + 1), BPoint(r, b), lighter);
		view->AddLine(BPoint(l + 1, b), BPoint(r, b), lighter);
		view->AddLine(BPoint(r - 1, t + 2), BPoint(r - 1, b - 1), light);
		view->AddLine(BPoint(l + 2, b - 1), BPoint(r - 1, b - 1), light);
		view->AddLine(BPoint(l, t), BPoint(l, b), shadow);
		view->AddLine(BPoint(l, t), BPoint(r, t), shadow);
		view->AddLine(BPoint(l + 1, t + 1), BPoint(l + 1, b - 1), dark);
		view->AddLine(BPoint(l + 1, t + 1), BPoint(r - 1, t + 1), dark);
	}
	else {
		view->AddLine(BPoint(l, t), BPoint(r - 1, t), lighter);
		view->AddLine(BPoint(l, t), BPoint(l, b - 1), lighter);
		view->AddLine(BPoint(l + 1, t + 1), BPoint(r - 2, t + 1), light);
		view->AddLine(BPoint(l + 1, t + 1), BPoint(l + 1, b - 2), light);
		view->AddLine(BPoint(l, b), BPoint(r, b), shadow);
		view->AddLine(BPoint(r, t), BPoint(r, b), shadow);
		view->AddLine(BPoint(l + 1, b - 1), BPoint(r - 1, b - 1), dark);
		view->AddLine(BPoint(r - 1, t + 1), BPoint(r - 1, b - 1), dark);
	}
	view->EndLineArray();

	view->PopState();
}
