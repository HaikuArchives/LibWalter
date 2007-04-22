/*
	BitmapListItem.h: A BListView item with an optional picture
	Written by DarkWyrm <darkwyrm@earthlink.net>, Copyright 2007
	Released under the MIT license.
	
	Certain code portions courtesy of the Haiku project
*/
#ifndef BITMAP_LIST_ITEM_H
#define BITMAP_LIST_ITEM_H

#include <ListItem.h>
#include <Bitmap.h>

class BitmapListItem : public BStringItem
{
public:
						BitmapListItem(BBitmap *bitmap, const char *text,
										uint32 level = 0,
										bool expanded = true);
						BitmapListItem(BMessage *data);
						~BitmapListItem(void);
			
	virtual	status_t	Archive(BMessage *data, bool deep = true) const;
	
	virtual	void		SetBitmap(BBitmap *bitmap);
			BBitmap *	Bitmap(void) const;
	virtual	void		DrawItem(BView *owner, BRect bounds, bool all = false);
	
private:
	BBitmap		*fBitmap;
	float		fBaselineOffset;
};

#endif
