// ============================================================================
//
// libwalter PictureButton.h
//
// Distributed under the terms of the MIT License.
//
// Authors:
//		Gabriele Biffi - http://www.biffuz.it/
//
// ============================================================================

#ifndef __PICTUREBUTTON_H__
#define __PICTUREBUTTON_H__

// Haiku headers
#include <Button.h>

// libwalter headers
#include "ButtonDecorator.h"

class PictureButton : public BButton {
private:
	BRect				_draw_default(BRect bounds, bool enabled);
	void				_draw_focus_line(float x, float y, float width,
							bool visible);
	float				fContentWidth, fContentHeight;
	float				fMinWidth, fMinHeight;
	ButtonDecorator *	fDecorator;
public:

			PictureButton(BRect frame, const char *name, const char *label,
				BBitmap *picture, BDPosition position, BMessage *message,
				uint32 resizingMode = B_FOLLOW_LEFT | B_FOLLOW_TOP,
    			uint32 flags = B_FULL_UPDATE_ON_RESIZE | B_NAVIGABLE |
    				B_WILL_DRAW);
    		PictureButton(BMessage *archive);
    virtual	~PictureButton();

	// Our methods
			ButtonDecorator *	Decorator(void);

			BBitmap *			Picture(void);
			BDPictureType		PictureType(void);
	virtual	bool				SetPicture(BBitmap *picture);

			BDPosition			Position(void);
	virtual	void				SetPosition(BDPosition position);
			BDVisible			Visible(void);
	virtual	void				SetVisible(BDVisible visible);

	// BArchivable hooks
	static	BArchivable *	Instantiate(BMessage *archive);
	virtual	status_t		Archive(BMessage *archive, bool deep = true) const;

   	// BButton hooks
	virtual	void	MakeDefault(bool flag);

	// BControl hooks
	virtual	void	SetLabel(const char *label);

	// BView hooks
    virtual	void	Draw(BRect updateRect);
	virtual	void	GetPreferredSize(float *width, float *height);
};

#endif // __PICTUREBUTTON_H__
