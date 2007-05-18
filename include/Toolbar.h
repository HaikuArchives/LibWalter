// =============================================================================
//
// libwalter Toolbar.h
//
// WToolbar, a toolbar widget
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

#ifndef _TOOLBAR_H_
#define _TOOLBAR_H_

// Standard C++ headers
#include <vector>

// BeOS headers
#include <View.h>

// libwalter classes
class WToolbarItem;

// Toolbar pre-defined styles
enum {
	W_TOOLBAR_STYLE_FLAT	= 0,
	W_TOOLBAR_STYLE_3D		= 1,
	W_TOOLBAR_STYLE_MENU	= 2
};

// Toolbar alignment
typedef enum {
	W_TOOLBAR_HORIZONTAL,
	W_TOOLBAR_VERTICAL
} WToolbarAlignment;

// Label position
typedef enum {
	W_TOOLBAR_LABEL_NONE,
	W_TOOLBAR_LABEL_BOTTOM,
	W_TOOLBAR_LABEL_SIDE
} WToolbarLabelPosition;

// Standard picture sizes
enum {
	W_TOOLBAR_PICTURE_NONE		=   0,
	W_TOOLBAR_PICTURE_XSMALL	=  16,
	W_TOOLBAR_PICTURE_SMALL		=  24,
	W_TOOLBAR_PICTURE_MEDIUM	=  32,
	W_TOOLBAR_PICTURE_LARGE		=  48,
	W_TOOLBAR_PICTURE_XLARGE	=  64,
	W_TOOLBAR_PICTURE_XXLARGE	=  96,
	W_TOOLBAR_PICTURE_XXXLARGE	= 128,
	W_TOOLBAR_PICTURE_HUGE		= 256,
	W_TOOLBAR_PICTURE_MAX		= 512
};

// Controls limit
#define W_TOOLBAR_MAX_ITEMS		1024

// Toolbar appearance
typedef struct {
	WToolbarAlignment		fAlignment;
	border_style			fBorder;
	BBitmap *				fBackBitmap;
	WToolbarLabelPosition	fLabelPosition;
	float					fMargin;
	float					fPadding;
	float					fPicSize;
	int						fStyle;
} WToolbarAppearance;

// =============================================================================
// WToolbar
// =============================================================================

class WToolbar : public BView {
private:
	typedef vector<WToolbarItem*> WToolbarItems;
			void					_draw_item(unsigned index,
										BRect updateRect);
			void					_init_object(void);
	// Appearance
			WToolbarAppearance		fAppearance;
	// Properties
			bool					fAutoSize;
			bool					fEnabled;
			BMessenger				fTarget;
	// Internals
			WToolbarItems			fItems;
			bool					fDisableUpdate;
			bool					fDisableStyling;
			WToolbarItem *			fMouseDownItem;
			WToolbarItem *			fMouseOverItem;
protected:
			void					AssertLocked(void);
	virtual	float					BorderSize(void);
	virtual	void					DrawBackground(BRect updateRect);
	virtual	void					DrawBorder(BRect updateRect);
public:
									WToolbar(BRect frame, const char *name,
										int style = W_TOOLBAR_STYLE_FLAT,
										WToolbarAlignment alignment =
										W_TOOLBAR_HORIZONTAL,
										bool auto_size = true,
										border_style border =
										B_FANCY_BORDER,
										uint32 resizing_mode =
										B_FOLLOW_LEFT | B_FOLLOW_TOP,
										uint32 flags = B_FRAME_EVENTS |
										B_FULL_UPDATE_ON_RESIZE |
										B_WILL_DRAW);
									WToolbar(BMessage *archive);
	virtual							~WToolbar();
	// BArchivable hooks
	virtual	status_t				Archive(BMessage *archive,
										bool deep = true) const;
	static	BArchivable *			Instantiate(BMessage *archive);
	// BHandler hooks
	//virtual	status_t			GetSupportedSuites(BMessage *message);
	virtual	void					MessageReceived(BMessage *message);
	//virtual	BHandler *			ResolveSpecifier(BMessage *message,
	//									int32 index, BMessage *specifier,
	//									int32 what, const char *property);
	// BView hooks
	virtual	void					AttachedToWindow(void);
	virtual	void					DetachedFromWindow(void);
			void					Draw(BRect updateRect);
			void					GetPreferredSize(float *width,
										float *height);
	virtual	void					MouseDown(BPoint point);
	virtual	void					MouseMoved(BPoint point, uint32 transit,
										const BMessage *message);
	virtual	void					MouseUp(BPoint point);
	// Items management
			bool					AddItem(WToolbarItem *item,
										int line = -1, int position = -1);
			WToolbarItem *			ItemAt(BPoint point);
			WToolbarItem *			ItemAt(int index);
			WToolbarItem *			ItemAt(int line, int position,
										bool exact = false);
			int						CountItemsInLine(int line);
			int						CountItems(void);
			int						CountLines(void);
			bool					DrawItem(WToolbarItem *item);
			WToolbarItem *			FindItem(const char *name);
			int						IndexOf(WToolbarItem *item);
			bool					RemoveItem(
										WToolbarItem *item);
	// Properties
			bool					AutoSize(void);
			bool					Enabled(void);
			void					SetAutoSize(bool auto_size);
			void					SetEnabled(bool enabled);
	// Appearance
			WToolbarAlignment		Alignment(void);
			BBitmap *				BackgroundBitmap(void);
			border_style			Border(void);
			WToolbarLabelPosition	LabelPosition(void);
			float					Margin(void);
			float					Padding(void);
			float					PictureSize(void);
			void					SetAlignment(
										WToolbarAlignment alignment);
			void					SetBackgroundBitmap(BBitmap *bitmap);
			void					SetBorder(border_style border);
			void					SetLabelPosition(WToolbarLabelPosition
										label_position);
			void					SetMargin(float margin);
			void					SetPadding(float padding);
			void					SetPictureSize(float picture_size);
	virtual	void					SetStyle(int style);
			int						Style(void);
	// Target
			bool					IsTargetLocal(void) const;
			BMessenger				Messenger(void) const;
			status_t				SetControlsTarget(BMessenger messenger);
			status_t				SetControlsTarget(const BHandler *handler,
										const BLooper *looper = NULL);
			status_t				SetTarget(BMessenger messenger);
			status_t				SetTarget(const BHandler *handler,
										const BLooper *looper = NULL);
			BHandler *				Target(BLooper **looper = NULL) const;
	// Other methods
			void					Update(void);
};

#endif // _TOOLBAR_H_
