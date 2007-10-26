/*
Open Tracker License

Terms and Conditions

Copyright (c) 1991-2000, Be Incorporated. All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice applies to all licensees
and shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF TITLE, MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
BE INCORPORATED BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF, OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of Be Incorporated shall not be
used in advertising or otherwise to promote the sale, use or other dealings in
this Software without prior written authorization from Be Incorporated.

Tracker(TM), Be(R), BeOS(R), and BeIA(TM) are trademarks or registered trademarks
of Be Incorporated in the United States and other countries. Other brand product
names are registered trademarks or trademarks of their respective holders.
All rights reserved.
*/

#include <Window.h>
#include "PaneSwitch.h"
#include "Thread.h"

const rgb_color kNormalColor = {150, 150, 150, 255};
const rgb_color kHighlightColor = {100, 100, 0, 255};

PaneSwitch::PaneSwitch(BRect frame, const char *name, bool leftAligned,
	uint32 resizeMask, uint32 flags)
	:	BControl(frame, name, "", 0, resizeMask, flags),
		fLeftAligned(leftAligned),
		fPressing(false)
{
}


void
PaneSwitch::DoneTracking(BPoint point)
{
	BRect bounds(Bounds());
	bounds.InsetBy(-3, -3);

	fPressing = false;
	Invalidate();
	if (bounds.Contains(point)) {
		SetValue(!Value());
		Invoke();
	}
}


void
PaneSwitch::Track(BPoint point, uint32)
{
	BRect bounds(Bounds());
	bounds.InsetBy(-3, -3);

	bool newPressing = bounds.Contains(point);
	if (newPressing != fPressing) {
		fPressing = newPressing;
		Invalidate();
	}
}


void
PaneSwitch::MouseDown(BPoint)
{
	if (!IsEnabled())
		return;

	fPressing = true;
	MouseDownThread<PaneSwitch>::TrackMouse(this, &PaneSwitch::DoneTracking,
		&PaneSwitch::Track);
	Invalidate();
}


void
PaneSwitch::Draw(BRect)
{
	if (fPressing)
		DrawInState(kPressed);
	else if (Value())
		DrawInState(kExpanded);
	else
		DrawInState(kCollapsed);

	rgb_color markColor = ui_color(B_KEYBOARD_NAVIGATION_COLOR);
	
	bool focused = IsFocus() && Window()->IsActive();
	BRect bounds(Bounds());
	BeginLineArray(2);
	AddLine(BPoint(bounds.left + 2, bounds.bottom - 1),
		BPoint(bounds.right - 2, bounds.bottom - 1), focused ? markColor : ViewColor());
	AddLine(BPoint(bounds.left + 2, bounds.bottom),
		BPoint(bounds.right - 2, bounds.bottom), focused ? kWhite : ViewColor());
	EndLineArray();
}


void
PaneSwitch::DrawInState(PaneSwitch::State state)
{
	BRect rect(0, 0, 10, 10);

	rgb_color outlineColor = {0, 0, 0, 255};
	rgb_color middleColor = state == kPressed ? kHighlightColor : kNormalColor;

	SetDrawingMode(B_OP_COPY);

	switch (state) {
		case kCollapsed:
			BeginLineArray(6);
			
			if (fLeftAligned) {
				AddLine(BPoint(rect.left + 3, rect.top + 1), 
					BPoint(rect.left + 3, rect.bottom - 1), outlineColor);
				AddLine(BPoint(rect.left + 3, rect.top + 1), 
					BPoint(rect.left + 7, rect.top + 5), outlineColor);
				AddLine(BPoint(rect.left + 7, rect.top + 5), 
					BPoint(rect.left + 3, rect.bottom - 1), outlineColor);

				AddLine(BPoint(rect.left + 4, rect.top + 3), 
					BPoint(rect.left + 4, rect.bottom - 3), middleColor);
				AddLine(BPoint(rect.left + 5, rect.top + 4), 
					BPoint(rect.left + 5, rect.bottom - 4), middleColor);
				AddLine(BPoint(rect.left + 5, rect.top + 5), 
					BPoint(rect.left + 6, rect.top + 5), middleColor);
			} else {
				AddLine(BPoint(rect.right - 3, rect.top + 1), 
					BPoint(rect.right - 3, rect.bottom - 1), outlineColor);
				AddLine(BPoint(rect.right - 3, rect.top + 1), 
					BPoint(rect.right - 7, rect.top + 5), outlineColor);
				AddLine(BPoint(rect.right - 7, rect.top + 5), 
					BPoint(rect.right - 3, rect.bottom - 1), outlineColor);

				AddLine(BPoint(rect.right - 4, rect.top + 3), 
					BPoint(rect.right - 4, rect.bottom - 3), middleColor);
				AddLine(BPoint(rect.right - 5, rect.top + 4), 
					BPoint(rect.right - 5, rect.bottom - 4), middleColor);
				AddLine(BPoint(rect.right - 5, rect.top + 5), 
					BPoint(rect.right - 6, rect.top + 5), middleColor);
			}
			EndLineArray();
			break;

		case kPressed:
			BeginLineArray(7);
			if (fLeftAligned) {
				AddLine(BPoint(rect.left + 1, rect.top + 7), 
					BPoint(rect.left + 7, rect.top + 7), outlineColor);
				AddLine(BPoint(rect.left + 7, rect.top + 1), 
					BPoint(rect.left + 7, rect.top + 7), outlineColor);
				AddLine(BPoint(rect.left + 1, rect.top + 7), 
					BPoint(rect.left + 7, rect.top + 1), outlineColor);

				AddLine(BPoint(rect.left + 3, rect.top + 6), 
					BPoint(rect.left + 6, rect.top + 6), middleColor);
				AddLine(BPoint(rect.left + 4, rect.top + 5), 
					BPoint(rect.left + 6, rect.top + 5), middleColor);
				AddLine(BPoint(rect.left + 5, rect.top + 4), 
					BPoint(rect.left + 6, rect.top + 4), middleColor);
				AddLine(BPoint(rect.left + 6, rect.top + 3), 
					BPoint(rect.left + 6, rect.top + 4), middleColor);
			} else {
				AddLine(BPoint(rect.right - 1, rect.top + 7), 
					BPoint(rect.right - 7, rect.top + 7), outlineColor);
				AddLine(BPoint(rect.right - 7, rect.top + 1), 
					BPoint(rect.right - 7, rect.top + 7), outlineColor);
				AddLine(BPoint(rect.right - 1, rect.top + 7), 
					BPoint(rect.right - 7, rect.top + 1), outlineColor);

				AddLine(BPoint(rect.right - 3, rect.top + 6), 
					BPoint(rect.right - 6, rect.top + 6), middleColor);
				AddLine(BPoint(rect.right - 4, rect.top + 5), 
					BPoint(rect.right - 6, rect.top + 5), middleColor);
				AddLine(BPoint(rect.right - 5, rect.top + 4), 
					BPoint(rect.right - 6, rect.top + 4), middleColor);
				AddLine(BPoint(rect.right - 6, rect.top + 3), 
					BPoint(rect.right - 6, rect.top + 4), middleColor);
			}
			EndLineArray();
			break;

		case kExpanded:
			BeginLineArray(6);
			AddLine(BPoint(rect.left + 1, rect.top + 3), 
				BPoint(rect.right - 1, rect.top + 3), outlineColor);
			AddLine(BPoint(rect.left + 1, rect.top + 3), 
				BPoint(rect.left + 5, rect.top + 7), outlineColor);
			AddLine(BPoint(rect.left + 5, rect.top + 7), 
				BPoint(rect.right - 1, rect.top + 3), outlineColor);

			AddLine(BPoint(rect.left + 3, rect.top + 4), 
				BPoint(rect.right - 3, rect.top + 4), middleColor);
			AddLine(BPoint(rect.left + 4, rect.top + 5), 
				BPoint(rect.right - 4, rect.top + 5), middleColor);
			AddLine(BPoint(rect.left + 5, rect.top + 5), 
				BPoint(rect.left + 5, rect.top + 6), middleColor);
			EndLineArray();
			break;
	}
}

