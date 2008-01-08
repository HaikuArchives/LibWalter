/*
 * Copyright 2007 Matthias Lindner, aka Paradoxon
 * Copyright 2007 Oliver Ruiz Dorantes
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include "Divider.h"
#include <Message.h>
#include <Locker.h>
#include <app/Cursor.h>
#include <stdio.h>

const uint32 B_DIVIDER_MOVED	= 'BDM';

static const int8 sHorizontalCursorData[68] = { 16,1,8,10,0,0,1,128,3,192,7,224,0,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,7,224,3,192,1,128,0,0,0,0,1,128,3,192,7,224,0,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,7,224,3,192,1,128,0,0 };
static const int8 sVerticalCursorData[68] = { 16,1,10,8,1,128,1,128,1,128,1,128,1,128,17,136,49,140,113,142,113,142,49,140,17,136,1,128,1,128,1,128,1,128,1,128,1,128,1,128,1,128,1,128,1,128,17,136,49,140,113,142,113,142,49,140,17,136,1,128,1,128,1,128,1,128,1,128 };

Divider::Divider(BView *first, BView *second, orientation dir)
  :	BBox(BRect(0,0,0,0),"Divider",B_FOLLOW_ALL_SIDES,B_WILL_DRAW),
	fDirection(dir),
	fMove(false),
	fFirstView(first),
	fSecondView(second),
	fHorizontalCursor(NULL),
	fVerticalCursor(NULL)
{
	// Set it true, so that the cursor with the first call of the method 
    // MouseMoved() will handle
	fDirectionModified = true;

	if (fDirection == B_HORIZONTAL) {
        // Create the Horizontal-Cursors, they remain permanently in _horizontalCursor stored.
        // _dragCursor is the current cursor, therefore MouseMoved() would later move it
        // if we a need again a Vertical Cursor(we still dont know here, because it is still not created).
		fHorizontalCursor = new BCursor(sHorizontalCursorData);
		fDragCursor = fHorizontalCursor;

		ResizeTo(fFirstView->Frame().Width(),10);
		MoveTo(0,fFirstView->Frame().bottom + 1);
		fFirstView->SetResizingMode(B_FOLLOW_LEFT_RIGHT);
		fSecondView->MoveTo(0,Frame().bottom + 1);
		fSecondView->SetResizingMode(B_FOLLOW_LEFT_RIGHT);
		SetResizingMode(B_FOLLOW_LEFT_RIGHT);
	} else {
        // Create the Vertical-Cursors, they remain permanently _verticalCursor stored
        // _dragCursor is the current cursor, therefore MouseMoved() would later move it
        // if we need again a HorizontalCursor(we still dont know here, because it is still not created).
		fVerticalCursor = new BCursor(sVerticalCursorData);
		fDragCursor = fVerticalCursor;
		
		ResizeTo(10,fFirstView->Frame().Height());
		MoveTo(fFirstView->Frame().right + 1,0);
		fFirstView->SetResizingMode(B_FOLLOW_TOP_BOTTOM);
		fSecondView->MoveTo(Frame().right + 1,0);
		fSecondView->SetResizingMode(B_FOLLOW_TOP_BOTTOM);
		SetResizingMode(B_FOLLOW_TOP_BOTTOM);
	}
}


Divider::~Divider(void)
{
}


float 
Divider::GetStart(void)
{
	return (fDirection == B_HORIZONTAL) ? Frame().top : Frame().left;
}


float
Divider::GetEnd(void)
{
	return (fDirection == B_HORIZONTAL) ? Frame().bottom : Frame().right;
}


float 
Divider::GetWidth(void)
{
	return (fDirection == B_HORIZONTAL) ? Bounds().Height() : Bounds().Width();
}


float 
Divider::GetLocation(void)
{
	return (fDirection == B_HORIZONTAL) ? (Frame().top + (Bounds().Height() / 2)) :
										(Frame().left + (Bounds().Width() / 2));
}


float 
Divider::GetMax(void)
{
	return (fDirection == B_HORIZONTAL) ? fSecondView->Frame().bottom :
										fSecondView->Frame().right;
}


float 
Divider::GetMin(void)
{
	return (fDirection == B_HORIZONTAL) ? fFirstView->Frame().top : fFirstView->Frame().left;
}


BView *
Divider::GetFirstView(void)
{
	return fFirstView;
}


BView *
Divider::GetSecondView(void)
{
	return fSecondView;
}


orientation
Divider::GetDirection(void)
{
	return fDirection;
}


void 
Divider::SetFirstView(BView *first)
{
	fFirstView = first;
	if (fDirection == B_HORIZONTAL) {
		ResizeTo(fFirstView->Frame().Width(),10);
		MoveTo(0,fFirstView->Frame().bottom + 1);
		fFirstView->SetResizingMode(B_FOLLOW_LEFT_RIGHT);
	} else {
		ResizeTo(10,fFirstView->Frame().Height());
		MoveTo(fFirstView->Frame().right + 1,0);
		fFirstView->SetResizingMode(B_FOLLOW_TOP_BOTTOM);
	}
}


void
Divider::SetSecondView(BView *second)
{
	fSecondView = second;
	if (fDirection == B_HORIZONTAL) {
		fSecondView->MoveTo(0,Frame().bottom + 1);
		fSecondView->SetResizingMode(B_FOLLOW_LEFT_RIGHT);
	} else {
		fSecondView->MoveTo(Frame().right + 1,0);
		fSecondView->SetResizingMode(B_FOLLOW_TOP_BOTTOM);  
	}
}


void
Divider::SetLocation(float location)
{
	if (location < GetMin()) 
		location = GetMin();
	else if (location > GetMax()) 
		location = GetMax();

	if (fDirection == B_HORIZONTAL) {
		float relative = location - (Frame().top + Bounds().Height() / 2);
		MoveBy(0,relative);
	} else {
		float relative = location - (Frame().left + Bounds().Width() / 2);
		MoveBy(relative,0);
	}  
}


void
Divider::SetDirection(orientation dir)
{
	float width = GetWidth();
	fDirection = dir;
	if (fDirection == B_HORIZONTAL) {
		ResizeTo(fFirstView->Bounds().Width(),width);
		MoveTo(0,fFirstView->Frame().bottom);
		fSecondView->MoveTo(0,Frame().bottom + 1);
		min = fFirstView->Frame().top + 5;
		max = fSecondView->Frame().bottom - 5;
		fFirstView->SetResizingMode(B_FOLLOW_LEFT_RIGHT);
		fSecondView->SetResizingMode(B_FOLLOW_LEFT_RIGHT);
		SetResizingMode(B_FOLLOW_LEFT_RIGHT);

		// In the case _horizontalCursor is not set in the constructor 
        // set it here
		if (!fHorizontalCursor)
			fHorizontalCursor = new BCursor(sHorizontalCursorData);

		// and set the current cursor from the _horizontalCursor
		fDragCursor = fHorizontalCursor;
		// and set the _directionModified-Flag so that MouseMoved() 
        // can make the next cursor change
		fDirectionModified = true;

	} else {
		ResizeTo(width,fFirstView->Bounds().Height());
		MoveTo(fFirstView->Frame().right,0);
		fSecondView->MoveTo(Frame().right,0);
		min = fFirstView->Bounds().left + 5;
		max = fSecondView->Bounds().right - 5;
		fFirstView->SetResizingMode(B_FOLLOW_TOP_BOTTOM);
		fSecondView->SetResizingMode(B_FOLLOW_TOP_BOTTOM);
		SetResizingMode(B_FOLLOW_TOP_BOTTOM);

		// In the case _verticalCursor is not set in the constructor 
        // set it here
		if (!fVerticalCursor)
			fVerticalCursor = new BCursor(sVerticalCursorData);
		
		// and set the current cursor from the _horizontalCursor
		fDragCursor = fVerticalCursor;
		// and set the _directionModified-Flag so that MouseMoved() 
        // can make the next cursor change
		fDirectionModified = true;
	}
}


void 
Divider::SetWidth(float width)
{
	if (fDirection == B_HORIZONTAL)
		ResizeTo(fFirstView->Bounds().Width(),width);
	else
		ResizeTo(width,fFirstView->Bounds().Height());
}


void 
Divider::MoveBy(float x, float y)
{
	if (x != 0) {
		// Is the limit on X to be exceeded or nothing to do
		if (((Frame().left + (x - 1)) > fFirstView->Frame().left)&&((Frame().right + x + 1) < fSecondView->Frame().right)) {
			// First move the pointer
			BBox::MoveBy(x,0);	// Then make our first View smaller
			fFirstView->ResizeBy(((Frame().left - 2) - fFirstView->Frame().right),0);
			// make smaller second view
			fSecondView->ResizeBy((fSecondView->Frame().left - (Frame().right + 1)),0);
			// and move it
			fSecondView->MoveTo(Frame().right + 1,0);
		}
	}
	
	if (y != 0) {
		// Look MoveByX for explanations
		if (((Frame().top + (y - 1)) > fFirstView->Frame().top)&&((Frame().bottom + y + 1) < fSecondView->Frame().bottom)) {
			BBox::MoveBy(0,y);
			Frame().bottom = Frame().bottom + y;
			Invalidate();
			fFirstView->ResizeBy(0,((Frame().top - 1) - fFirstView->Frame().bottom));
			fSecondView->ResizeBy(0,(fSecondView->Frame().top - (Frame().bottom + 1)));
			fSecondView->MoveTo(0,Frame().bottom + 1);
		}
	}
}


void
Divider::MouseDown(BPoint point)
{
	uint32 buttons;
	GetMouse(&point, &buttons);
	if (buttons & B_PRIMARY_MOUSE_BUTTON) {
		mx = point.x;
		my = point.y;  
		fMove = true;
		SetMouseEventMask(B_POINTER_EVENTS,B_LOCK_WINDOW_FOCUS | B_NO_POINTER_HISTORY);
	}
	/* Keep were we started the movement */
	fStartingMovement = GetLocation();
}


void
Divider::MouseMoved(BPoint point, uint32 transit, const BMessage *message)
{
	// Just when the direction has changed, set the cursor
	if ( fDirectionModified ) {
		SetViewCursor(fDragCursor, true);
		fDirectionModified = false;
	}
	if (fMove) {
		if (fDirection == B_HORIZONTAL)
			MoveBy(0,point.y - my);  
		else
			MoveBy(point.x - mx,0);
   }
}


void Divider::MouseUp(BPoint point)
{
	printf("up %f:%f\n",point.x,point.y);
	
	if (fMove) {
		fMove = false;
		// 	 SetMouseEventMask(B_POINTER_EVENTS,B_LOCK_WINDOW_FOCUS | B_NO_POINTER_HISTORY);
	}
	if (fStartingMovement == GetLocation()) {
		/* We have not moved anything... seems to be a click 
		   move to the previous stored location */
		SetLocation(fWhereWere);
		/* And the starting one now is the previous */
		fWhereWere = fStartingMovement;
	} else {
		fWhereWere = fStartingMovement;
	}
}
