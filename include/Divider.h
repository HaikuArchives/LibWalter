/*
 * Copyright 2007 Matthias Lindner, aka Paradoxon
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#ifndef DIVIDER_H
#define DIVIDER_H

#include <Box.h>
#include <Cursor.h>

const uint32 B_DIVIDER_WIDTH    = 10;

class Divider : public BBox {

public:
    Divider(BView *lView,BView *rView,orientation dr);

		float	    GetStart();
		float	    GetEnd();
		float	    GetWidth();
		float	    GetLocation();
		float	    GetMax();
		float	    GetMin();
		BView*      GetFirstView();
		BView*      GetSecondView();
		orientation	GetDirection();
		void		SetFirstView(BView *fView);
        void		SetSecondView(BView *sView);
        void		SetLocation(float location);
        void 		SetDirection(orientation dr);
        void		SetWidth(float width);
        void		MouseDown(BPoint point);
virtual void		MouseMoved(BPoint point, uint32 transit, const BMessage *message);
        void		MouseUp(BPoint point);
        void		MoveBy(float x,float y);
        void		MoveByX(float x);
        void		MoveByY(float y);
private:
        orientation	direction;
        bool		move;

        // Dieses Flag wird benutzt, um der MouseMoved()-Methode mitzuteilen, 
        // dass sich die Direction geändert hat, und sie den Cursor neu setzen soll
        bool		_directionModified;
        float		relative,max,min,mx,my;
        BView*      firstView;
        BView*      secondView;

        // In these atributes are stored the cursors
        BCursor*    _horizontalCursor;
        BCursor*    _verticalCursor;
        BCursor*    _dragCursor;

};

#endif
