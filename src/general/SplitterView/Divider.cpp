/*
 * Copyright 2007 Matthias Lindner, aka Paradoxon
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include "Divider.h"
#include <Message.h>
#include <Locker.h>
#include <Cursor.h>
#include <stdio.h>

const uint32 B_DIVIDER_MOVED    = 'BDM';

Divider::Divider(BView *fV,BView *sV,orientation dr) : 
    BBox(BRect(0,0,0,0),"Divider",B_FOLLOW_ALL_SIDES,B_WILL_DRAW)
{
    direction = dr;
    firstView = fV;
    secondView = sV;
    move = false;
    _horizontalCursor= _verticalCursor = 0;

    // Hier auf true setzen, damit der Cursor beim ersten Aufruf der MouseMoved()-Methode gesetzt wird.
    _directionModified = true;

    if (direction==B_HORIZONTAL){

        // Erzeugen des Horizontal-Cursors, dieser bleibt in alle Ewigkeit in _horizontalCursor gespeichert
        // _dragCursor ist der aktuelle Cursor, den MouseMoved() später verwenden soll
        // ob wir einen Vertical Cursor noch einmal brauchen, wissen wir hier noch nicht, deshalb wird er noch nicht erzeugt.
        int8 cursordata[68] = {16,1,8,10,0,0,1,128,3,192,7,224,0,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,7,224,3,192,1,128,0,0,0,0,1,128,3,192,7,224,0,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,7,224,3,192,1,128,0,0};

        _horizontalCursor = new BCursor(cursordata);
        _dragCursor = _horizontalCursor;

        ResizeTo(firstView->Frame().Width(),10);
        MoveTo(0,firstView->Frame().bottom+1);
        firstView->SetResizingMode(B_FOLLOW_LEFT_RIGHT);
        secondView->MoveTo(0,Frame().bottom+1);
        secondView->SetResizingMode(B_FOLLOW_LEFT_RIGHT);
        SetResizingMode(B_FOLLOW_LEFT_RIGHT);
    }
    else{ 

        // Erzeugen des Vertical-Cursors, dieser bleibt in alle Ewigkeit in _verticalCursor gespeichert
        // _dragCursor ist der aktuelle Cursor, den MouseMoved() später verwenden soll
        // ob wir einen HorizontalCursor noch einmal brauchen, wissen wir hier noch nicht, deshalb wird er noch nicht erzeugt.

        int8 cursordata[68] = {16,1,10,8,1,128,1,128,1,128,1,128,1,128,17,136,49,140,113,142,113,142,49,140,17,136,1,128,1,128,1,128,1,128,1,128,1,128,1,128,1,128,1,128,1,128,17,136,49,140,113,142,113,142,49,140,17,136,1,128,1,128,1,128,1,128,1,128};
        _verticalCursor = new BCursor(cursordata);
        _dragCursor = _verticalCursor;

        ResizeTo(10,firstView->Frame().Height());
        MoveTo(firstView->Frame().right+1,0);
        firstView->SetResizingMode(B_FOLLOW_TOP_BOTTOM);
        secondView->MoveTo(Frame().right+1,0);
        secondView->SetResizingMode(B_FOLLOW_TOP_BOTTOM);
        SetResizingMode(B_FOLLOW_TOP_BOTTOM);
    }
}


float 
Divider::GetStart()
{
    if (direction==B_HORIZONTAL){
        return Frame().top;
    }
    else{
        return Frame().left;
    }
}


float
Divider::GetEnd()
{
    if (direction==B_HORIZONTAL){
        return Frame().bottom;
    }
    else{
        return Frame().right;
    }
}


float 
Divider::GetWidth()
{
    if (direction==B_HORIZONTAL){
        return Bounds().Height();
    }
    else{
        return Bounds().Width();
    }
}


float 
Divider::GetLocation()
{
    if (direction==B_HORIZONTAL){
        return (Frame().top+(Bounds().Height()/2));
    }
    else{
        return (Frame().left+(Bounds().Width()/2));
    }
}


float 
Divider::GetMax()
{
    if (direction==B_HORIZONTAL)
        return secondView->Frame().bottom;
    else
        return secondView->Frame().right;
}


float 
Divider::GetMin()
{
    if (direction==B_HORIZONTAL)
        return firstView->Frame().top;
    else
        return firstView->Frame().left;
}


BView*
Divider::GetFirstView()
{
    return firstView;
}


BView*
Divider::GetSecondView()
{
    return secondView;
}


orientation
Divider::GetDirection()
{
    return direction;
}


void 
Divider::SetFirstView(BView* fView)
{
    firstView=fView;
    if (direction==B_HORIZONTAL){
        ResizeTo(firstView->Frame().Width(),10);
        MoveTo(0,firstView->Frame().bottom+1);
        firstView->SetResizingMode(B_FOLLOW_LEFT_RIGHT);
    }
    else{
        ResizeTo(10,firstView->Frame().Height());
        MoveTo(firstView->Frame().right+1,0);
        firstView->SetResizingMode(B_FOLLOW_TOP_BOTTOM);
    }
}


void
Divider::SetSecondView(BView *sView)
{
    secondView=sView;
    if (direction==B_HORIZONTAL){
        secondView->MoveTo(0,Frame().bottom+1);
        secondView->SetResizingMode(B_FOLLOW_LEFT_RIGHT);
    }
    else{
        secondView->MoveTo(Frame().right+1,0);
        secondView->SetResizingMode(B_FOLLOW_TOP_BOTTOM);  
    }
}


void
Divider::SetLocation(float location)
{
    if (location<GetMin()) 
        location=GetMin();
    else if (location>GetMax()) 
        location=GetMax();

    if (direction==B_HORIZONTAL){
        float relative=location-(Frame().top+Bounds().Height()/2);
        MoveBy(0,relative);
    }
    else{
        float relative=location-(Frame().left+Bounds().Width()/2);
        MoveBy(relative,0);
    }  
}


void
Divider::SetDirection(orientation dr)
{
    float width=GetWidth();
    direction=dr;
    if (direction==B_HORIZONTAL){
        ResizeTo(firstView->Bounds().Width(),width);
        MoveTo(0,firstView->Frame().bottom);
        secondView->MoveTo(0,Frame().bottom+1);
        min=firstView->Frame().top+5;
        max=secondView->Frame().bottom-5;
        firstView->SetResizingMode(B_FOLLOW_LEFT_RIGHT);
        secondView->SetResizingMode(B_FOLLOW_LEFT_RIGHT);
        SetResizingMode(B_FOLLOW_LEFT_RIGHT);

        // Falls der _horizontalCursor noch nicht im Konstruktor gesetzt wurde, tun wir es hier
        if (_horizontalCursor == 0) {
		    int8 cursordata[68] = {16,1,8,10,0,0,1,128,3,192,7,224,0,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,7,224,3,192,1,128,0,0,0,0,1,128,3,192,7,224,0,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,7,224,3,192,1,128,0,0};
		    _horizontalCursor = new BCursor(cursordata);
        }

        // und setzten den aktuellen Cursor auf den _horizontalCursor 
		_dragCursor = _horizontalCursor;
        // und setzen das _directionModified-Flag damit MouseMoved() beim nächsten mal den Cursor ändert
		_directionModified = true;

    }
    else{
        ResizeTo(width,firstView->Bounds().Height());
        MoveTo(firstView->Frame().right,0);
        secondView->MoveTo(Frame().right,0);
        min=firstView->Bounds().left+5;
        max=secondView->Bounds().right-5;
        firstView->SetResizingMode(B_FOLLOW_TOP_BOTTOM);
        secondView->SetResizingMode(B_FOLLOW_TOP_BOTTOM);
        SetResizingMode(B_FOLLOW_TOP_BOTTOM);

        // Falls der _verticalCursor noch nicht im Konstruktor gesetzt wurde, tun wir es hier
	    if (_verticalCursor == 0) {
		    int8 cursordata[68] = {16,1,10,8,1,128,1,128,1,128,1,128,1,128,17,136,49,140,113,142,113,142,49,140,17,136,1,128,1,128,1,128,1,128,1,128,1,128,1,128,1,128,1,128,1,128,17,136,49,140,113,142,113,142,49,140,17,136,1,128,1,128,1,128,1,128,1,128};
		    _verticalCursor = new BCursor(cursordata);
	    }
        
        // und setzten den aktuellen Cursor auf den _verticalCursor 
		_dragCursor = _verticalCursor;
        // und setzen das _directionModified-Flag damit MouseMoved() beim nächsten mal den Cursor ändert
		_directionModified = true;

    }
}


void 
Divider::SetWidth(float width)
{
    if (direction==B_HORIZONTAL)
        ResizeTo(firstView->Bounds().Width(),width);
    else
        ResizeTo(width,firstView->Bounds().Height());
}

void 
Divider::MoveByX(float x)
{
    //werden bei x die Grenzen überschritten oder soll überhaupt nichts gemacht werden???
    if (((Frame().left+(x-1))>firstView->Frame().left)&&((Frame().right+x+1)<secondView->Frame().right)){
        //erst Pointer selber bewegen..
        BBox::MoveBy(x,0);      //dann unser ersten View kleiner machen...
        firstView->ResizeBy(((Frame().left-2)-firstView->Frame().right),0);
        //zweiten View kleiner machen
        secondView->ResizeBy((secondView->Frame().left-(Frame().right+1)),0);
        //und entsprechend verschieben
        secondView->MoveTo(Frame().right+1,0);
    }
}


void
Divider::MoveByY(float y)
{
    //Erklärung siehe MoveByX;
    if (((Frame().top+(y-1))>firstView->Frame().top)&&((Frame().bottom+y+1)<secondView->Frame().bottom)){
        BBox::MoveBy(0,y);
        Frame().bottom=Frame().bottom+y;
        Invalidate();
        firstView->ResizeBy(0,((Frame().top-1)-firstView->Frame().bottom));
        secondView->ResizeBy(0,(secondView->Frame().top-(Frame().bottom+1)));
        secondView->MoveTo(0,Frame().bottom+1);
    }
}


void 
Divider::MoveBy(float x, float y)
{
    if (x!=0)
        MoveByX(x);
    if (y!=0)
        MoveByY(y);
}


void
Divider::MouseDown(BPoint point)
{
    uint32 buttons;
    GetMouse(&point, &buttons);
    if (buttons & B_PRIMARY_MOUSE_BUTTON) {
        mx=point.x;
        my=point.y;  
        move=true;
        SetMouseEventMask(B_POINTER_EVENTS,B_LOCK_WINDOW_FOCUS | B_NO_POINTER_HISTORY);
    }
}


void
Divider::MouseMoved(BPoint point, uint32 transit, const BMessage *message)
{
    //Nur wenn sich die Richtung geändert hat, setzten wir den Cursor
    if ( _directionModified ) {
        SetViewCursor(_dragCursor, true);
        _directionModified = false;
    }
    if (move){
        if (direction==B_HORIZONTAL)
            MoveByY(point.y-my);  
        else
            MoveByX(point.x-mx);  
   }
}


void Divider::MouseUp(BPoint point)
{
    if (move) {
        move=false;
        // 	 SetMouseEventMask(B_POINTER_EVENTS,B_LOCK_WINDOW_FOCUS | B_NO_POINTER_HISTORY);
    }
}
