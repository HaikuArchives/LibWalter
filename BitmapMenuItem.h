/*
	BitmapListItem.h: A BMenuItem with an optional picture
	Written by DarkWyrm <darkwyrm@earthlink.net>, Copyright 2007
	Released under the MIT license.
*/
#ifndef BITMAP_MENU_ITEM_H
#define BITMAP_MENU_ITEM_H

#include <MenuItem.h>

class BitmapMenuItem : public BMenuItem
{
public:
						BitmapMenuItem(const char *label, BMessage *msg,
										BBitmap *bitmap, char shortcut = 0,
										uint32 modifiers = 0);
			 			BitmapMenuItem(BMessage *data);
	virtual 			~BitmapMenuItem(void);
	virtual	status_t	Archive(BMessage *data, bool deep = true) const;
	
	virtual	void		GetContentSize(float *width, float *height);	
	virtual	void		DrawContent(void);
	
	virtual	void		SetBitmap(BBitmap *bitmap);
			BBitmap *	Bitmap(void) const;
	
private:
	BBitmap				*fBitmap;
	float				fBaselineOffset;
};

#endif
