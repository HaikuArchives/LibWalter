// =============================================================================
//
// libwalter ToolbarSupport.cpp
//
// Miscellaneous functions for Toolbar and related classes
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

#ifndef _TOOLBAR_SUPPORT_H_
#define _TOOLBAR_SUPPORT_H_

// BeOS headers
#include <Rect.h>
#include <SupportDefs.h>

// BeOS classes
class BBitmap;
class BDirectory;
class BFile;
class BMenu;
class BTextView;
class BView;
struct entry_ref;

// =============================================================================
// ToolbarSupport
// =============================================================================

class ToolbarSupport {
public:
	static	BBitmap *		GrayscaleBitmap(BBitmap *bitmap);
	static	void			Draw3DBorder(BView *view, BRect rect,
								bool inset = false);
};

#endif // _TOOLBAR_SUPPORT_H_
