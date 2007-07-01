/*
 * Copyright 2007 Matthias Lindner, aka Paradoxon
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include "Divider.h"
#include <Message.h>
#include <Locker.h>
#include <Cursor.h>
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
	// Hier auf true setzen, damit der Cursor beim ersten Aufruf der MouseMoved()-Methode gesetzt wird.
	fDirectionModified = true;

	if (fDirection == B_HORIZONTAL) {
		// Erzeugen des Horizontal-Cursors, dieser bleibt in alle Ewigkeit in fHorizontalCursor gespeichert
		// fDragCursor ist der aktuelle Cursor, den MouseMoved() später verwenden soll
		// ob wir einen Vertical Cursor noch einmal brauchen, wissen wir hier noch nicht, deshalb wird er noch nicht erzeugt.
		fHorizontalCursor = new BCursor(sHorizontalCursorData);
		fDragCursor = fHorizontalCursor;

		ResizeTo(fFirstView->Frame().Width(),10);
		MoveTo(0,fFirstView->Frame().bottom + 1);
		fFirstView->SetResizingMode(B_FOLLOW_LEFT_RIGHT);
		fSecondView->MoveTo(0,Frame().bottom + 1);
		fSecondView->SetResizingMode(B_FOLLOW_LEFT_RIGHT);
		SetResizingMode(B_FOLLOW_LEFT_RIGHT);
	} else {
		// Erzeugen des Vertical-Cursors, dieser bleibt in alle Ewigkeit in fVerticalCursor gespeichert
		// fDragCursor ist der aktuelle Cursor, den MouseMoved() später verwenden soll
		// ob wir einen HorizontalCursor noch einmal brauchen, wissen wir hier noch nicht, deshalb wird er noch nicht erzeugt.
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

		// Falls der fHorizontalCursor noch nicht im Konstruktor gesetzt wurde, tun wir es hier
		if (!fHorizontalCursor)
			fHorizontalCursor = new BCursor(sHorizontalCursorData);

		// und setzten den aktuellen Cursor auf den fHorizontalCursor 
		fDragCursor = fHorizontalCursor;
		// und setzen das fDirectionModified-Flag damit MouseMoved() beim nächsten mal den Cursor ändert
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

		// Falls der fVerticalCursor noch nicht im Konstruktor gesetzt wurde, tun wir es hier
		if (!fVerticalCursor)
			fVerticalCursor = new BCursor(sVerticalCursorData);
		
		// und setzten den aktuellen Cursor auf den fVerticalCursor 
		fDragCursor = fVerticalCursor;
		// und setzen das fDirectionModified-Flag damit MouseMoved() beim nächsten mal den Cursor ändert
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
		//werden bei x die Grenzen überschritten oder soll überhaupt nichts gemacht werden???
		if (((Frame().left + (x - 1)) > fFirstView->Frame().left)&&((Frame().right + x + 1) < fSecondView->Frame().right)) {
			//erst Pointer selber bewegen..
			BBox::MoveBy(x,0);	//dann unser ersten View kleiner machen...
			fFirstView->ResizeBy(((Frame().left - 2) - fFirstView->Frame().right),0);
			//zweiten View kleiner machen
			fSecondView->ResizeBy((fSecondView->Frame().left - (Frame().right + 1)),0);
			//und entsprechend verschieben
			fSecondView->MoveTo(Frame().right + 1,0);
		}
	}
	
	if (y != 0) {
		//Erklärung siehe MoveByX;
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
}


void
Divider::MouseMoved(BPoint point, uint32 transit, const BMessage *message)
{
	//Nur wenn sich die Richtung geändert hat, setzten wir den Cursor
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
	if (fMove) {
		fMove = false;
		// 	 SetMouseEventMask(B_POINTER_EVENTS,B_LOCK_WINDOW_FOCUS | B_NO_POINTER_HISTORY);
	}
}
