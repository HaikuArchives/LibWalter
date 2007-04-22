/*
	Spinner.cpp: A number spinner control.
	Written by DarkWyrm <darkwyrm@earthlink.net>, Copyright 2004
	Released under the MIT license.
	
	Original BScrollBarButton class courtesy Haiku project
*/
#include "Spinner.h"
#include <String.h>
#include <ScrollBar.h>
#include <Window.h>
#include <stdio.h>
#include <Font.h>
#include <Box.h>
#include <MessageFilter.h>


enum {
	M_UP = 'mmup',
	M_DOWN,
	M_TEXT_CHANGED = 'mtch'
};


typedef enum {
	ARROW_LEFT = 0,
	ARROW_RIGHT,
	ARROW_UP,
	ARROW_DOWN,
	ARROW_NONE
} arrow_direction;


class SpinnerMsgFilter : public BMessageFilter
{
public:
							SpinnerMsgFilter(void);
							~SpinnerMsgFilter(void);
	virtual filter_result	Filter(BMessage *msg, BHandler **target);
};


class SpinnerArrowButton : public BView
{
public:
							SpinnerArrowButton(BPoint location, const char *name, 
								arrow_direction dir);
							~SpinnerArrowButton(void);
					void	AttachedToWindow(void);
					void	DetachedToWindow(void);
					void	MouseDown(BPoint pt);
					void	MouseUp(BPoint pt);
					void	MouseMoved(BPoint pt, uint32 code, const BMessage *msg);
					void	Draw(BRect update);
					void	SetEnabled(bool value);
					bool	IsEnabled(void) const { return fEnabled; }
	
private:
	arrow_direction		fDirection;
	BPoint				fTrianglePoint1,
						fTrianglePoint2,
						fTrianglePoint3;
	Spinner				*fParent;
	bool				fMouseDown;
	bool				fEnabled;
};

class SpinnerPrivateData
{
public:
	SpinnerPrivateData(void)
	{
		fThumbFrame.Set(0,0,0,0);
		fEnabled = true;
		tracking = false;
		fMousePoint.Set(0,0);
		fThumbIncrement = 1.0;
		fRepeaterID = -1;
		fExitRepeater = false;
		fArrowDown = ARROW_NONE;
		
		#ifdef TEST_MODE
			sbinfo.proportional = true;
			sbinfo.double_arrows = false;
			sbinfo.knob = 0;
			sbinfo.min_knob_size = 14;
		#else
			get_scroll_bar_info(&fScrollbarInfo);
		#endif
	}
	
	~SpinnerPrivateData(void)
	{
		if (fRepeaterID != -1)
		{
			fExitRepeater = false;
			kill_thread(fRepeaterID);
		}
	}
	
	static	int32	ButtonRepeaterThread(void *data);
	
			thread_id 		fRepeaterID;
			scroll_bar_info	fScrollbarInfo;
			BRect			fThumbFrame;
			bool			fEnabled;
			bool			tracking;
			BPoint			fMousePoint;
			float			fThumbIncrement;
			bool			fExitRepeater;
			arrow_direction	fArrowDown;
};


Spinner::Spinner(BRect frame, const char *name, const char *label, BMessage *msg,
				uint32 resize,uint32 flags)
 :	BControl(frame,name,label,msg,resize,flags),
	fStep(1),
	fMin(0),
	fMax(100)
{
	_InitObject();
}


Spinner::Spinner(BMessage *data)
 :	BControl(data)
{
	if (data->FindInt32("_min",&fMin) != B_OK)
		fMin = 0;
	if (data->FindInt32("_max",&fMax) != B_OK)
		fMin = 100;
	if (data->FindInt32("_step",&fStep) != B_OK)
		fMin = 1;
	_InitObject();
}


Spinner::~Spinner(void)
{
	delete fPrivateData;
	delete fFilter;
}


void
Spinner::_InitObject(void)
{
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	BRect r(Bounds());
	if (r.Height() < B_H_SCROLL_BAR_HEIGHT * 2)
		r.bottom = r.top + 1 + B_H_SCROLL_BAR_HEIGHT * 2;
	ResizeTo(r.Width(),r.Height());
	
	r.right -= B_V_SCROLL_BAR_WIDTH;
	
	font_height fh;
	BFont font;
	font.GetHeight(&fh);
	float textheight = fh.ascent + fh.descent + fh.leading;
	
	r.top = 0;
	r.bottom = textheight;
	
	fTextControl = new BTextControl(r,"textcontrol",Label(),"0",
									new BMessage(M_TEXT_CHANGED), B_FOLLOW_TOP | 
									B_FOLLOW_LEFT_RIGHT,
									B_WILL_DRAW | B_NAVIGABLE);
	AddChild(fTextControl);
	fTextControl->ResizeTo(r.Width(), MAX(textheight, fTextControl->TextView()->LineHeight(0) + 4.0));
	fTextControl->MoveTo(0,
		((B_H_SCROLL_BAR_HEIGHT * 2) - fTextControl->Bounds().Height()) / 2);
		
	fTextControl->SetDivider(StringWidth(Label()) + 5);
	
	BTextView *tview = fTextControl->TextView();
	tview->SetAlignment(B_ALIGN_LEFT);
	tview->SetWordWrap(false);
	
	BString string("QWERTYUIOP[]\\ASDFGHJKL;'ZXCVBNM,/qwertyuiop{}| "
		"asdfghjkl:\"zxcvbnm<>?!@#$%^&*()-_=+`~\r");
	
	for (int32 i = 0; i < string.CountChars(); i++) {
		char c = string.ByteAt(i);
		tview->DisallowChar(c);
	}
	
	r = Bounds();
	r.left = r.right - B_V_SCROLL_BAR_WIDTH;
	r.bottom = B_H_SCROLL_BAR_HEIGHT;
	
	fUpButton = new SpinnerArrowButton(r.LeftTop(),"up",ARROW_UP);
	AddChild(fUpButton);
	
	r.OffsetBy(0,r.Height() + 1);
	fDownButton = new SpinnerArrowButton(r.LeftTop(),"down",ARROW_DOWN);
	AddChild(fDownButton);
	
	
	fPrivateData = new SpinnerPrivateData;
	fFilter = new SpinnerMsgFilter;
}


BArchivable *
Spinner::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "Spinner"))
		return new Spinner(data);

	return NULL;
}


status_t
Spinner::Archive(BMessage *data, bool deep) const
{
	status_t status = BControl::Archive(data, deep);
	
	if (status == B_OK)
		status = data->AddInt32("_min",fMin);
	
	if (status == B_OK)
		status = data->AddInt32("_max",fMax);
	
	if (status == B_OK)
		status = data->AddInt32("_step",fStep);
	
	return status;
}


void
Spinner::AttachedToWindow(void)
{
	Window()->AddCommonFilter(fFilter);
	fTextControl->SetTarget(this);
}


void
Spinner::DetachedFromWindow(void)
{
	Window()->RemoveCommonFilter(fFilter);
}


void
Spinner::SetValue(int32 value)
{
	if (value > GetMax() || value < GetMin())
		return;
	
	BControl::SetValue(value);
	
	char string[50];
	sprintf(string,"%ld",value);
	fTextControl->SetText(string);
}


void
Spinner::SetLabel(const char *text)
{
	fTextControl->SetLabel(text);
}


void
Spinner::ValueChanged(int32 value)
{
}


void
Spinner::MessageReceived(BMessage *msg)
{
	if (msg->what == M_TEXT_CHANGED) {
		BString string(fTextControl->Text());
		int32 newvalue = 0;
		
		sscanf(string.String(),"%ld",&newvalue);
		if (newvalue >= GetMin() && newvalue <= GetMax()) {
			// new value is in range, so set it and go
			SetValue(newvalue);
			Invoke();
			Draw(Bounds());
			ValueChanged(Value());
		} else {
			// new value is out of bounds. Clip to range if current value is not
			// at the end of its range
			if (newvalue < GetMin() && Value() != GetMin()) {
				SetValue(GetMin());
				Invoke();
				Draw(Bounds());
				ValueChanged(Value());
			} else if (newvalue>GetMax() && Value() != GetMax()) {
				SetValue(GetMax());
				Invoke();
				Draw(Bounds());
				ValueChanged(Value());
			} else {
				char string[100];
				sprintf(string,"%ld",Value());
				fTextControl->SetText(string);
			}
		}
	}
	else
		BControl::MessageReceived(msg);
}


void
Spinner::GetPreferredSize(float *width, float *height)
{
	float w, h;
	
	// This code shamelessly copied from the Haiku TextControl.cpp and adapted
	
	// we need enough space for the label and the child text view
	font_height fontHeight;
	fTextControl->GetFontHeight(&fontHeight);
	float labelHeight = ceil(fontHeight.ascent + fontHeight.descent + fontHeight.leading);
	float textHeight = fTextControl->TextView()->LineHeight(0) + 4.0;

	h = max_c(labelHeight, textHeight);
	
	w = 25.0f + ceilf(fTextControl->StringWidth(fTextControl->Label())) + 
		ceilf(fTextControl->StringWidth(fTextControl->Text()));
	
	w += B_V_SCROLL_BAR_WIDTH;
	if (h < fDownButton->Frame().bottom)
		h = fDownButton->Frame().bottom;
	
	if (width)
		*width = w;
	if (height)
		*height = h;
}


void
Spinner::ResizeToPreferred(void)
{
	float w,h;
	GetPreferredSize(&w,&h);
	ResizeTo(w,h);
}


void
Spinner::SetSteps(int32 stepsize)
{
	fStep = stepsize;
}


void
Spinner::SetRange(int32 min, int32 max)
{
	SetMin(min);
	SetMax(max);
}


void
Spinner::GetRange(int32 *min, int32 *max)
{
	*min = fMin;
	*max = fMax;
}


void
Spinner::SetMax(int32 max)
{
	fMax = max;
	if (Value() > fMax)
		SetValue(fMax);
}


void
Spinner::SetMin(int32 min)
{
	fMin = min;
	if (Value() < fMin)
		SetValue(fMin);
}


void
Spinner::SetEnabled(bool value)
{
	if (IsEnabled() == value)
		return;
	
	BControl::SetEnabled(value);
	fTextControl->SetEnabled(value);
	fUpButton->SetEnabled(value);
	fDownButton->SetEnabled(value);
}


void
Spinner::MakeFocus(bool value)
{
	fTextControl->MakeFocus(value);
}


int32
SpinnerPrivateData::ButtonRepeaterThread(void *data)
{
	Spinner *sp = (Spinner *)data;
	
	snooze(250000);
	
	bool exitval = false;
	
	sp->Window()->Lock();
	exitval = sp->fPrivateData->fExitRepeater;
	
	int32 scrollvalue = 0;
	if (sp->fPrivateData->fArrowDown == ARROW_UP)
		scrollvalue = sp->fStep;
	else if (sp->fPrivateData->fArrowDown != ARROW_NONE)
		scrollvalue = -sp->fStep;
	else
		exitval = true;
	
	sp->Window()->Unlock();
	
	while (!exitval) {
		sp->Window()->Lock();
		
		int32 newvalue = sp->Value() + scrollvalue;
		if (newvalue >= sp->GetMin() && newvalue <= sp->GetMax()) {
			// new value is in range, so set it and go
			sp->SetValue(newvalue);
			sp->Invoke();
			sp->Draw(sp->Bounds());
			sp->ValueChanged(sp->Value());
		} else {
			// new value is out of bounds. Clip to range if current value is not
			// at the end of its range
			if (newvalue<sp->GetMin() && sp->Value() != sp->GetMin()) {
				sp->SetValue(sp->GetMin());
				sp->Invoke();
				sp->Draw(sp->Bounds());
				sp->ValueChanged(sp->Value());
			} else  if (newvalue>sp->GetMax() && sp->Value() != sp->GetMax())
			{
				sp->SetValue(sp->GetMax());
				sp->Invoke();
				sp->Draw(sp->Bounds());
				sp->ValueChanged(sp->Value());
			}
		}
		sp->Window()->Unlock();
		
		snooze(50000);
		
		sp->Window()->Lock();
		exitval = sp->fPrivateData->fExitRepeater;
		sp->Window()->Unlock();
	}
	
	sp->Window()->Lock();
	sp->fPrivateData->fExitRepeater = false;
	sp->fPrivateData->fRepeaterID = -1;
	sp->Window()->Unlock();
	return 0;
	exit_thread(0);
}


SpinnerArrowButton::SpinnerArrowButton(BPoint location, const char *name,
										arrow_direction dir)
 :BView(BRect(0,0,B_V_SCROLL_BAR_WIDTH - 1,B_H_SCROLL_BAR_HEIGHT - 1).OffsetToCopy(location),
 		name, B_FOLLOW_RIGHT | B_FOLLOW_TOP, B_WILL_DRAW)
{
	fDirection = dir;
	fEnabled = true;
	BRect r = Bounds();
	
	switch (fDirection) {
		case ARROW_LEFT: {
			fTrianglePoint1.Set(r.left + 3,(r.top + r.bottom) / 2);
			fTrianglePoint2.Set(r.right - 3,r.top + 3);
			fTrianglePoint3.Set(r.right - 3,r.bottom - 3);
			break;
		}
		case ARROW_RIGHT: {
			fTrianglePoint1.Set(r.left + 3,r.bottom - 3);
			fTrianglePoint2.Set(r.left + 3,r.top + 3);
			fTrianglePoint3.Set(r.right - 3,(r.top + r.bottom) / 2);
			break;
		}
		case ARROW_UP: {
			fTrianglePoint1.Set(r.left + 3,r.bottom - 3);
			fTrianglePoint2.Set((r.left + r.right) / 2,r.top + 3);
			fTrianglePoint3.Set(r.right - 3,r.bottom - 3);
			break;
		}
		default: {
			fTrianglePoint1.Set(r.left + 3,r.top + 3);
			fTrianglePoint2.Set(r.right - 3,r.top + 3);
			fTrianglePoint3.Set((r.left + r.right) / 2,r.bottom - 3);
			break;
		}
	}
	fMouseDown = false;
}


SpinnerArrowButton::~SpinnerArrowButton(void)
{
}


void
SpinnerArrowButton::MouseDown(BPoint pt)
{
	if (fEnabled == false)
		return;
	
/*	fMouseDown = true;
	Draw(Bounds());

	if (fParent)
	{
		int32 step = fParent->GetSteps();
				
		int32 newvalue = fParent->Value();
		
		if (fDirection == ARROW_UP)
		{
			fParent->fPrivateData->fArrowDown = ARROW_UP;
			newvalue += step;
		}
		else
		{
			fParent->fPrivateData->fArrowDown = ARROW_DOWN;
			newvalue -= step;
		}

		if ( newvalue >= fParent->GetMin() && newvalue <= fParent->GetMax())
		{
			// new value is in range, so set it and go
			fParent->SetValue(newvalue);
			fParent->Invoke();
			fParent->Draw(fParent->Bounds());
			fParent->ValueChanged(fParent->Value());
		}
		else
		{
			// new value is out of bounds. Clip to range if current value is not
			// at the end of its range
			if (newvalue<fParent->GetMin() && fParent->Value() != fParent->GetMin())
			{
				fParent->SetValue(fParent->GetMin());
				fParent->Invoke();
				fParent->Draw(fParent->Bounds());
				fParent->ValueChanged(fParent->Value());
			}
			else
			if (newvalue>fParent->GetMax() && fParent->Value() != fParent->GetMax())
			{
				fParent->SetValue(fParent->GetMax());
				fParent->Invoke();
				fParent->Draw(fParent->Bounds());
				fParent->ValueChanged(fParent->Value());
			}
			else
			{
				// cases which go here are if new value is <minimum and value already at
				// minimum or if > maximum and value already at maximum
				return;
			}
		}

		if (fParent->fPrivateData->fRepeaterID == -1)
		{
			fParent->fPrivateData->fExitRepeater = false;
			fParent->fPrivateData->fRepeaterID = spawn_thread(fParent->fPrivateData->ButtonRepeaterThread,
				"scroll repeater",B_NORMAL_PRIORITY,fParent);
			resume_thread(fParent->fPrivateData->fRepeaterID);
		}
	}
*/
	if (!IsEnabled())
		return;

	// TODO: Handle asynchronous window controls flag
//	if (Window()->Flags() & B_ASYNCHRONOUS_CONTROLS)
//	{
//		SetTracking(true);
//	SetMouseEventMask(B_POINTER_EVENTS, B_LOCK_WINDOW_FOCUS);
// 	}
// 	else
 //	{
//		BRect bounds = Bounds();
		uint32 buttons;
		BPoint point;

		int32 step = fParent->GetSteps();
		
		int32 newvalue = fParent->Value();
		
		int32 waitvalue = 250000;
		
		do {
			if (fDirection == ARROW_UP) {
				fParent->fPrivateData->fArrowDown = ARROW_UP;
				newvalue += step;
			} else {
				fParent->fPrivateData->fArrowDown = ARROW_DOWN;
				newvalue -= step;
			}
			
			if (newvalue >= fParent->GetMin() && newvalue <= fParent->GetMax()) {
				// new value is in range, so set it and go
				fParent->SetValue(newvalue);
				fParent->Invoke();
//				fParent->Draw(fParent->Bounds());
				fParent->ValueChanged(fParent->Value());
			} else {
				// new value is out of bounds. Clip to range if current value is not
				// at the end of its range
				if (newvalue < fParent->GetMin() && 
						fParent->Value() != fParent->GetMin()) {
					fParent->SetValue(fParent->GetMin());
					fParent->Invoke();
//					fParent->Draw(fParent->Bounds());
					fParent->ValueChanged(fParent->Value());
				} else if (newvalue>fParent->GetMax() && 
							fParent->Value() != fParent->GetMax()) {
					fParent->SetValue(fParent->GetMax());
					fParent->Invoke();
//					fParent->Draw(fParent->Bounds());
					fParent->ValueChanged(fParent->Value());
				} else {
					// cases which go here are if new value is <minimum and value already at
					// minimum or if > maximum and value already at maximum
					return;
				}
			}
			
			Window()->UpdateIfNeeded();
			
			snooze(waitvalue);
			
			GetMouse(&point, &buttons, true);
			
// 			bool inside = bounds.Contains(point);
			
//			if ((Value() == B_CONTROL_ON) != inside)
//				SetValue(inside ? B_CONTROL_ON : B_CONTROL_OFF);
			
			if (waitvalue > 150000)
				waitvalue = 150000;
				
		} while (buttons != 0);

//	}

}


void
SpinnerArrowButton::MouseUp(BPoint pt)
{
	if (fEnabled) {
		fMouseDown = false;
		
		if (fParent) {
			fParent->fPrivateData->fArrowDown = ARROW_NONE;
			fParent->fPrivateData->fExitRepeater = true;
		}
		Draw(Bounds());
	}
}


void
SpinnerArrowButton::MouseMoved(BPoint pt, uint32 transit, const BMessage *msg)
{
	if (fEnabled == false)
		return;
	
	if (transit == B_ENTERED_VIEW) {
		BPoint point;
		uint32 buttons;
		GetMouse(&point,&buttons);
		if ((buttons & B_PRIMARY_MOUSE_BUTTON) == 0 &&
			(buttons & B_SECONDARY_MOUSE_BUTTON) == 0 &&
			(buttons & B_PRIMARY_MOUSE_BUTTON) == 0 )
			fMouseDown = false;
		else
			fMouseDown = true;
		Draw(Bounds());
	}
	
	if (transit == B_EXITED_VIEW || transit == B_OUTSIDE_VIEW)
		MouseUp(Bounds().LeftTop());
}


void
SpinnerArrowButton::Draw(BRect update)
{
	rgb_color c = ui_color(B_PANEL_BACKGROUND_COLOR);
	BRect r(Bounds());
	
	rgb_color light, dark, normal,arrow,arrow2;
	if (fMouseDown) {	
		light = tint_color(c,B_DARKEN_3_TINT);
		arrow2 = dark = tint_color(c,B_LIGHTEN_MAX_TINT);
		normal = c;
		arrow = tint_color(c,B_DARKEN_MAX_TINT);
	} else if (fEnabled) {
		arrow2 = light = tint_color(c,B_LIGHTEN_MAX_TINT);
		dark = tint_color(c,B_DARKEN_3_TINT);
		normal = c;
		arrow = tint_color(c,B_DARKEN_MAX_TINT);
	} else {
		arrow2 = light = tint_color(c,B_LIGHTEN_1_TINT);
		dark = tint_color(c,B_DARKEN_1_TINT);
		normal = c;
		arrow = tint_color(c,B_DARKEN_1_TINT);
	}
	
	r.InsetBy(1,1);
	SetHighColor(normal);
	FillRect(r);
	
	SetHighColor(arrow);
	FillTriangle(fTrianglePoint1,fTrianglePoint2,fTrianglePoint3);
	
	r.InsetBy(-1,-1);
	SetHighColor(dark);
	StrokeLine(r.LeftBottom(),r.RightBottom());
	StrokeLine(r.RightTop(),r.RightBottom());
	StrokeLine(fTrianglePoint2,fTrianglePoint3);
	StrokeLine(fTrianglePoint1,fTrianglePoint3);
	
	SetHighColor(light);
	StrokeLine(r.LeftTop(),r.RightTop());
	StrokeLine(r.LeftTop(),r.LeftBottom());
	
	SetHighColor(arrow2);
	StrokeLine(fTrianglePoint1,fTrianglePoint2);
}


void
SpinnerArrowButton::AttachedToWindow(void)
{
	fParent = (Spinner*)Parent();
}


void
SpinnerArrowButton::DetachedToWindow(void)
{
	fParent = NULL;
}


void
SpinnerArrowButton::SetEnabled(bool value)
{
	fEnabled = value;
	Invalidate();
}


SpinnerMsgFilter::SpinnerMsgFilter(void)
 : BMessageFilter(B_PROGRAMMED_DELIVERY, B_ANY_SOURCE,B_KEY_DOWN)
{
}


SpinnerMsgFilter::~SpinnerMsgFilter(void)
{
}


filter_result
SpinnerMsgFilter::Filter(BMessage *msg, BHandler **target)
{
	int32 c;
	msg->FindInt32("raw_char",&c);
	switch (c) {
		case B_ENTER: {
			BTextView *text = dynamic_cast<BTextView*>(*target);
			if (text && text->IsFocus()) {
				BView *view = text->Parent();
				while (view) {					
					Spinner *spin = dynamic_cast<Spinner*>(view);
					if (spin) {
						BString string(text->Text());
						int32 newvalue = 0;
						
						sscanf(string.String(),"%ld",&newvalue);
						if (newvalue != spin->Value()) {
							spin->SetValue(newvalue);
							spin->Invoke();
						}
						return B_SKIP_MESSAGE;
					}
					view = view->Parent();
				}
			}
			return B_DISPATCH_MESSAGE;
		}
		case B_TAB: {
			// Cause Tab characters to perform keybaord navigation
			BHandler *h = *target;
			BView *v = NULL;
			
			h = h->NextHandler();
			while (h) {
				v = dynamic_cast<BView*>(h);
				if (v) {
					*target = v->Window();
					return B_DISPATCH_MESSAGE;
				}
				h = h->NextHandler();
			}
			return B_SKIP_MESSAGE;
		}
		case B_UP_ARROW:
		case B_DOWN_ARROW: {
			BTextView *text = dynamic_cast<BTextView*>(*target);
			if (text && text->IsFocus()) {
				// We have a text view. See if it currently has the focus and belongs
				// to a Spinner control. If so, change the value of the spinner
				
				// TextViews are complicated beasts which are actually multiple views.
				// Travel up the hierarchy to see if any of the target's ancestors are
				// a Spinner.
				
				BView *view = text->Parent();
				while (view) {					
					Spinner *spin = dynamic_cast<Spinner*>(view);
					if (spin) {
						int32 step = spin->GetSteps();
						if (c == B_DOWN_ARROW)
							step = 0 - step;
						
						spin->SetValue(spin->Value() + step);
						spin->Invoke();
						return B_SKIP_MESSAGE;
					}
					view = view->Parent();
				}
			}
			
			return B_DISPATCH_MESSAGE;
		}
		default:
			return B_DISPATCH_MESSAGE;
	}
	
	// shut the stupid compiler up
	return B_SKIP_MESSAGE;
}
