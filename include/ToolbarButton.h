// =============================================================================
//
// libwalter ToolbarButton.h
//
// Simple button control for Toolbar
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

#ifndef _TOOLBAR_BUTTON_H_
#define _TOOLBAR_BUTTON_H_

// BeOS headers
#include <Control.h> // For B_CONTROL_ON and B_CONTROL_OFF

// libwalter headers
#include "ToolbarControl.h"

// BeOS classes
class BBitmap;

// =============================================================================
// ToolbarButton
// =============================================================================

class ToolbarButton : public ToolbarControl {
private:
			void				_calc_size(float &lh, float &lw, float &ph,
									float &pw);
			void				_init_object(void);
			char *				fLabel;
			bool				fSwitchMode;
			int32				fValue;
protected:
	// Infos for drawing hooks
			BView *				fCanvas;
			int					fStyle;
	// Drawing hooks
	virtual	float				BorderSize(void);
	virtual	void				DrawBackground(BRect updateRect);
	virtual	void				DrawBorder(BRect updateRect);
	virtual	void				DrawLabel(BPoint position, BRect updateRect);
	virtual	void				DrawPicture(BPoint position, BRect updateRect);
	virtual	void				GetLabelSize(float *width, float *height);
	virtual	void				GetPictureSize(float *width, float *height);
	virtual	float				Padding(void);
public:
								ToolbarButton(const char *name,
									const char *label,
									BMessage *message = NULL,
									bool switchMode = false);
								ToolbarButton(BMessage *archive);
	virtual						~ToolbarButton();
	// BArchivable hooks
	virtual	status_t			Archive(BMessage *archive,
									bool deep = true) const;
	static	BArchivable *		Instantiate(BMessage *archive);
	// AKToolbarControl hooks
			void				Draw(BView *canvas, BRect updateRect);
			void				GetPreferredSize(float *width, float *height);
			void				MouseUp(BPoint point);
	// Other methods
	static	BBitmap *			GetMenuCheckMark(void);
			const char *		Label(void);
			void				SetLabel(const char *label);
			void				SetValue(int32 value);
			void				SetSwitchMode(bool switchMode);
			bool				SwitchMode(void);
			int32				Value(void);
};

#endif // _TOOLBAR_BUTTON_H_
