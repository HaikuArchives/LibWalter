/*
 * Copyright 2007 Matthias Lindner, aka Paradoxon
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#ifndef DIVIDER_H
#define DIVIDER_H

#include <Box.h>
#include <Cursor.h>

#define B_DIVIDER_WIDTH	10

class Divider : public BBox
{
public:
					Divider(BView *lView, BView *rView, orientation dr);
		virtual		~Divider(void);
					
		float		GetStart(void);
		float		GetEnd(void);
		float		GetMax(void);
		float		GetMin(void);
		
		orientation	GetDirection(void);
		void 		SetDirection(orientation dr);
		
		void		SetFirstView(BView *fView);
		BView *		GetFirstView(void);
		
		void		SetSecondView(BView *sView);
		BView *		GetSecondView(void);
		
		void		SetLocation(float location);
		float		GetLocation(void);
		
		void		SetWidth(float width);
		float		GetWidth(void);
		
		void		MouseDown(BPoint point);
virtual void		MouseMoved(BPoint point, uint32 transit, const BMessage *message);
		void		MouseUp(BPoint point);
		
		void		MoveBy(float x,float y);

private:
		orientation	fDirection;
		bool		fMove;

		// This flag will be used in the method MouseMoved() to let know, 
		// that the direction has changed, therefore the cursor as well
		bool		fDirectionModified;
		float		relative,max,min,mx,my;
		BView		*fFirstView;
		BView		*fSecondView;
		
		BCursor		*fHorizontalCursor;
		BCursor		*fVerticalCursor;
		BCursor		*fDragCursor;
		
		float		fStartingMovement;
		float		fWhereWere;		
		
};

#endif
