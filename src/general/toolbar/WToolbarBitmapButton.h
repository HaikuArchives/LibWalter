// =============================================================================
//
// libwalter WToolbarBitmapButton.h
//
// Bitmap button control for WToolbar
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

#ifndef _W_TOOLBAR_BITMAP_BUTTON_H_
#define _W_TOOLBAR_BITMAP_BUTTON_H_

// Standard C++ headers
#include <vector>

// libwalter headers
#include "WToolbarButton.h"

// BeOS classes
class BBitmap;

// Bitmap set for the button
class WTBBBitmapSet : public BArchivable {
private:
			void			_init_object(void);
public:
							WTBBBitmapSet(void);
							WTBBBitmapSet(WTBBBitmapSet &set);
							WTBBBitmapSet(BMessage *message);
	virtual					~WTBBBitmapSet();
	// BArchivable hooks
	virtual	status_t		Archive(BMessage *archive, bool deep = true) const;
	static	BArchivable *	Instantiate(BMessage *archive);
	// Public data members
			unsigned		size;
			BBitmap *		normal;
			BBitmap *		disabled;
			BBitmap *		over;
			BBitmap *		over_disabled;
			BBitmap *		pushed;
			BBitmap *		pushed_disabled;
			BBitmap *		pushed_over;
			BBitmap *		pushed_over_disabled;
};

// =============================================================================
// WToolbarBitmapButton
// =============================================================================

class WToolbarBitmapButton : public WToolbarButton {
private:
			typedef std::vector<WTBBBitmapSet*> WTBBBitmapSets;
			void				_init_object(void);
			WTBBBitmapSets		fBitmaps;
protected:
	// WToolbarButton hooks
	virtual	void				DrawPicture(BPoint position, BRect updateRect);
	virtual	void				GetPictureSize(float *width, float *height);

public:
								WToolbarBitmapButton(const char *name,
									const char *label,
									BMessage *message = NULL);
								WToolbarBitmapButton(BMessage *archive);
	virtual						~WToolbarBitmapButton();
	// BArchivable hooks
	virtual	status_t			Archive(BMessage *archive,
									bool deep = true) const;
	static	BArchivable *		Instantiate(BMessage *archive);
	// Bitmaps
			bool				AddBitmapSet(BBitmap *normal,
									BBitmap *disabled = NULL,
									BBitmap *pushed = NULL,
									BBitmap *pushed_disabled = NULL,
									BBitmap *over = NULL,
									BBitmap *over_disabled = NULL,
									BBitmap *pushed_over = NULL,
									BBitmap *pushed_over_disabled = NULL);
			unsigned			CountBitmapSets(void);
			WTBBBitmapSet *		GetBitmapSet(unsigned size);
			WTBBBitmapSet *		GetBitmapSetAt(unsigned index);
			WTBBBitmapSet *		RemoveBitmapSet(unsigned size);
};

#endif // _W_TOOLBAR_BITMAP_BUTTON_H_
