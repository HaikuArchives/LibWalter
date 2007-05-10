// =============================================================================
//
// libwalter ToolbarBitmapButton.h
//
// Bitmap button control for Toolbar
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

#ifndef _TOOLBAR_BITMAP_BUTTON_H_
#define _TOOLBAR_BITMAP_BUTTON_H_

// Standard C++ headers
#include <vector>

// libwalter headers
#include "ToolbarButton.h"

// BeOS classes
class BBitmap;

// Bitmap set for the button
class TBBBitmapSet : public BArchivable {
private:
			void			_init_object(void);
public:
							TBBBitmapSet(void);
							TBBBitmapSet(TBBBitmapSet &set);
							TBBBitmapSet(BMessage *message);
	virtual					~TBBBitmapSet();
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
// ToolbarBitmapButton
// =============================================================================

class ToolbarBitmapButton : public ToolbarButton {
private:
			typedef std::vector<TBBBitmapSet*> TBBBitmapSets;
			void				_init_object(void);
			TBBBitmapSets		fBitmaps;
protected:
	// ToolbarButton hooks
	virtual	void				DrawPicture(BPoint position, BRect updateRect);
	virtual	void				GetPictureSize(float *width, float *height);

public:
								ToolbarBitmapButton(const char *name,
									const char *label,
									BMessage *message = NULL);
								ToolbarBitmapButton(BMessage *archive);
	virtual						~ToolbarBitmapButton();
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
			TBBBitmapSet *		GetBitmapSet(unsigned size);
			TBBBitmapSet *		GetBitmapSetAt(unsigned index);
			TBBBitmapSet *		RemoveBitmapSet(unsigned size);
};

#endif // _TOOLBAR_BITMAP_BUTTON_H_
