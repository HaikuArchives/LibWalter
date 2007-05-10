// =============================================================================
//
// libwalter ToolbarControl.h
//
// Base class for Toolbar controls
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

#ifndef _TOOLBAR_CONTROL_H_
#define _TOOLBAR_CONTROL_H_

// BeOS headers
#include <Handler.h>
#include <Invoker.h>
#include <Point.h>
#include <Rect.h>

// BeOS classes
class BView;

// libwalter classes
class Toolbar;

// =============================================================================
// ToolbarControl
// =============================================================================

class ToolbarControl : public BHandler, public BInvoker {
friend class Toolbar;
private:
			void			_init_object(void);
			bool			fEnabled;
			BRect			fFrame;
			float			fHeight;
			int				fLine;
			Toolbar *		fToolbar;
			bool			fVisible;
			float			fWidth;
protected:
			bool			fMouseDown;
			bool			fMouseOver;
public:
							ToolbarControl(const char *name = NULL,
								BMessage *message = NULL);
							ToolbarControl(BMessage *archive);
	virtual					~ToolbarControl();
	// BArchivable hooks
	virtual	status_t		Archive(BMessage *archive,
								bool deep = true) const;
	static	BArchivable *	Instantiate(BMessage *archive);
	// Other methods
	virtual	void			AttachedToToolbar(void);
			BRect			Bounds();
	virtual	void			DetachedFromToolbar(void);
	virtual	void			Draw(BView *canvas, BRect updateRect);
			bool			Enabled(void);
			BRect			Frame(void);
	virtual	void			GetPreferredSize(float *width,
								float *height);
			void			Invalidate(void);
			int				Line(void);
	virtual	void			MouseDown(BPoint point);
	virtual	void			MouseMoved(BPoint point, uint32 transit,
								const BMessage *message);
	virtual	void			MouseUp(BPoint point);
			int				Position(void);
			void			SetEnabled(bool enabled);
			void			SetVisible(bool visible);
			Toolbar *		ParentToolbar(void);
			bool			Visible(void);
};

#endif // _TOOLBAR_CONTROL_H_
