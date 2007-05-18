// =============================================================================
//
// libwalter ToolbarItem.h
//
// Base class for WToolbar items
//
// Revision: 20070518
// Page width 80, tab width 4, encoding UTF-8, line endings LF.
//
// Original author:
//     Gabriele Biffi - http://www.biffuz.it/
// Contributors:
//
// Released under the terms of the MIT license.
//
// =============================================================================

#ifndef _TOOLBAR_ITEM_H_
#define _TOOLBAR_ITEM_H_

// BeOS headers
#include <Handler.h>
#include <Point.h>
#include <Rect.h>

// BeOS classes
class BView;

// libwalter classes
class WToolbar;

// =============================================================================
// WToolbarItem
// =============================================================================

class WToolbarItem : public BHandler {
friend class WToolbar;
private:
			void			_init_object(void);
			BRect			fFrame;
			float			fHeight;
			int				fLine;
			WToolbar *		fToolbar;
			bool			fVisible;
			float			fWidth;
protected:
			bool			fMouseDown;
			bool			fMouseOver;
public:
							WToolbarItem(const char *name = NULL);
							WToolbarItem(BMessage *archive);
	virtual					~WToolbarItem();
	// BArchivable hooks
	virtual	status_t		Archive(BMessage *archive,
								bool deep = true) const;
	static	BArchivable *	Instantiate(BMessage *archive);
	// Other methods
	virtual	void			AttachedToToolbar(void);
			BRect			Bounds();
	virtual	void			DetachedFromToolbar(void);
	virtual	void			Draw(BView *canvas, BRect updateRect);
			BRect			Frame(void);
	virtual	void			GetPreferredSize(float *width,
								float *height);
			int				Index(void);
			void			Invalidate(void);
			int				Line(void);
	virtual	void			MouseDown(BPoint point);
	virtual	void			MouseMoved(BPoint point, uint32 transit,
								const BMessage *message);
	virtual	void			MouseUp(BPoint point);
			int				Position(void);
			void			SetVisible(bool visible);
			WToolbar *		Toolbar(void);
			bool			Visible(void);
};

#endif // _TOOLBAR_ITEM_H_
