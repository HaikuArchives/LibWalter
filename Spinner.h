/*
	Spinner.h: A number spinner control.
	Written by DarkWyrm <bpmagic@columbus.rr.com>, Copyright 2004
	Released under the MIT license.
	
	Original BScrollBarButton class courtesy Haiku project
*/
#ifndef SPINNER_H_
#define SPINNER_H_

#include <Control.h>
#include <TextView.h>
#include <Button.h>
#include <StringView.h>
#include <TextControl.h>

class SpinnerPrivateData;
class SpinnerArrowButton;
class SpinnerMsgFilter;

class Spinner : public BControl
{
public:
							Spinner(BRect frame, const char *name,
							const char *label, BMessage *msg,
							uint32 resize = B_FOLLOW_LEFT | B_FOLLOW_TOP,
							uint32 flags = B_WILL_DRAW | B_NAVIGABLE);
	virtual					~Spinner(void);
	virtual void			AttachedToWindow(void);
	virtual void			DetachedFromWindow(void);
	virtual void			ValueChanged(int32 value);
	virtual void			MessageReceived(BMessage *msg);
	
	virtual void			SetSteps(int32 stepsize);
			int32			GetSteps(void) const { return fStep; }
	
	virtual void			SetRange(int32 min, int32 max);
			void			GetRange(int32 *min, int32 *max);
	
	virtual	void			SetMax(int32 max);
			int32			GetMax(void) const { return fMax; }
	virtual	void			SetMin(int32 min);
			int32			GetMin(void) const { return fMin; }
	
	virtual	void			MakeFocus(bool value=true);
	
	virtual	void			SetValue(int32 value);
	virtual	void			SetLabel(const char *text);
			BTextControl *	TextControl(void) const { return fTextControl; }
	
	virtual	void			SetEnabled(bool value);
	
private:
	friend	SpinnerArrowButton;
	friend	SpinnerPrivateData;
	
	BTextControl		*fTextControl;
	SpinnerArrowButton	*fUpButton,
						*fDownButton;
	SpinnerPrivateData	*privatedata;
	
	int32				fStep;
	int32				fMin,
						fMax;
	SpinnerMsgFilter	*fFilter;
};

#endif
