// =============================================================================
//
// libwalter Toolbar.h
//
// Toolbar, a toolbar widget
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

#ifndef _TOOLBAR_H_
#define _TOOLBAR_H_

// Standard C++ headers
#include <vector>

// BeOS headers
#include <View.h>

// libwalter classes
class ToolbarControl;

// Toolbar pre-defined styles
enum {
	TOOLBAR_STYLE_FLAT	= 0,
	TOOLBAR_STYLE_3D		= 1,
	TOOLBAR_STYLE_MENU	= 2
};

// Toolbar alignment
typedef enum {
	TOOLBAR_HORIZONTAL,
	TOOLBAR_VERTICAL
} ToolbarAlignment;

// Label position
typedef enum {
	TOOLBAR_LABEL_NONE,
	TOOLBAR_LABEL_BOTTOM,
	TOOLBAR_LABEL_SIDE
} ToolbarLabelPosition;

// Standard picture sizes
enum {
	TOOLBAR_PICTURE_NONE		=   0,
	TOOLBAR_PICTURE_XSMALL	=  16,
	TOOLBAR_PICTURE_SMALL		=  24,
	TOOLBAR_PICTURE_MEDIUM	=  32,
	TOOLBAR_PICTURE_LARGE		=  48,
	TOOLBAR_PICTURE_XLARGE	=  64,
	TOOLBAR_PICTURE_XXLARGE	=  96,
	TOOLBAR_PICTURE_XXXLARGE	= 128,
	TOOLBAR_PICTURE_HUGE		= 256,
	TOOLBAR_PICTURE_MAX		= 512
};

// Controls limit
#define TOOLBAR_MAX_CONTROLS		1024

// Toolbar appearance
typedef struct {
	ToolbarAlignment		fAlignment;
	border_style			fBorder;
	BBitmap *				fBackBitmap;
	ToolbarLabelPosition	fLabelPosition;
	float					fMargin;
	float					fPadding;
	float					fPicSize;
	int						fStyle;
} ToolbarAppearance;

// =============================================================================
// Toolbar
// =============================================================================

class Toolbar : public BView {
private:
	typedef vector<ToolbarControl*> ToolbarControls;
			void					_draw_control(unsigned index,
										BRect updateRect);
			void					_init_object(void);
	// Appearance
			ToolbarAppearance		fAppearance;
	// Properties
			bool					fAutoSize;
			bool					fEnabled;
			BMessenger				fTarget;
	// Internals
			ToolbarControls		fControls;
			bool					fDisableUpdate;
			bool					fDisableStyling;
			ToolbarControl *		fDownControl;
			ToolbarControl *		fOverControl;
protected:
			void					AssertLocked(void);
	virtual	float					BorderSize(void);
	virtual	void					DrawBackground(BRect updateRect);
	virtual	void					DrawBorder(BRect updateRect);
public:
									Toolbar(BRect frame, const char *name,
										int style = TOOLBAR_STYLE_FLAT,
										ToolbarAlignment alignment =
										TOOLBAR_HORIZONTAL,
										bool auto_size = true,
										border_style border =
										B_FANCY_BORDER,
										uint32 resizing_mode =
										B_FOLLOW_LEFT | B_FOLLOW_TOP,
										uint32 flags = B_FRAME_EVENTS |
										B_FULL_UPDATE_ON_RESIZE |
										B_WILL_DRAW);
									Toolbar(BMessage *archive);
	virtual							~Toolbar();
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
	// Controls management
			bool					AddControl(ToolbarControl *control,
										int line = -1, int position = -1);
			ToolbarControl *		ControlAt(BPoint point);
			ToolbarControl *		ControlAt(int index);
			ToolbarControl *		ControlAt(int line, int position,
										bool exact = false);
			int						ControlsInLine(int line);
			int						CountControls(void);
			int						CountLines(void);
			bool					DrawControl(ToolbarControl *control);
			ToolbarControl *		FindControl(const char *name);
			int						IndexOf(ToolbarControl *control);
			bool					RemoveControl(
										ToolbarControl *control);
	// Properties
			bool					AutoSize(void);
			bool					Enabled(void);
			void					SetAutoSize(bool auto_size);
			void					SetEnabled(bool enabled);
	// Appearance
			ToolbarAlignment		Alignment(void);
			BBitmap *				BackgroundBitmap(void);
			border_style			Border(void);
			ToolbarLabelPosition	LabelPosition(void);
			float					Margin(void);
			float					Padding(void);
			float					PictureSize(void);
			void					SetAlignment(
										ToolbarAlignment alignment);
			void					SetBackgroundBitmap(BBitmap *bitmap);
			void					SetBorder(border_style border);
			void					SetLabelPosition(ToolbarLabelPosition
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
