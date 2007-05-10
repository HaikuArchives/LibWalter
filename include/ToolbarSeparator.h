// =============================================================================
//
// libwalter ToolbarSeparator.h
//
// Separator control for AKToolbar
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

#ifndef _TOOLBAR_SEPARATOR_H_
#define _TOOLBAR_SEPARATOR_H_

// libwalter headers
#include "ToolbarControl.h"

// =============================================================================
// ToolbarSeparator
// =============================================================================

class ToolbarSeparator : public ToolbarControl {
public:
							ToolbarSeparator(const char *name = NULL);
							ToolbarSeparator(BMessage *archive);
	virtual					~ToolbarSeparator();
	// BArchivable hooks
	virtual	status_t		Archive(BMessage *archive,
								bool deep = true) const;
	static	BArchivable *	Instantiate(BMessage *archive);
	// AKToolbarControl hooks
	virtual	void			Draw(BView *canvas, BRect updateRect);
	virtual	void			GetPreferredSize(float *width, float *height);
};

#endif // _TOOLBAR_SEPARATOR_H_
