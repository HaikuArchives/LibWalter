/*
 * Copyright 2007 Matthias Lindner, aka Paradoxon
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include "SplitterView.h"
#include <Message.h>
#include <stdio.h>

// TODO: Add a Direction() function

SplitterView::SplitterView(const BRect &frame, const orientation &dir, const uint32 &resizingMode,
							const uint32 &flags)
  :	BView(frame,"ViewSplitter", resizingMode,flags),
	fDirection(dir),
	fAnz(0),
	fStepWidth(0.0f)
{
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
}


void 
SplitterView::AddChild(BView *view)
{
	uint32 i;
	fAnz++;
	if (fDirection == B_HORIZONTAL) {
		fStepWidth = Bounds().Height() / (fAnz);
		view->ResizeTo(Bounds().Width(),fStepWidth - 10);
		if (fAnz > 1)
			ChildAt(CountChildren() - 1)->ResizeTo(Bounds().Width(),fStepWidth - 10);
	} else {
		fStepWidth = Bounds().Width() / (fAnz);
		view->ResizeTo(fStepWidth - 10,Bounds().Height());
		if (fAnz > 1)
			ChildAt(CountChildren() - 1)->ResizeTo(fStepWidth - 10,Bounds().Height());
	}
	
	if (fAnz == 1) {
		view->MoveTo(0,0);
		// adjustment because with only one view, there is no Divider
		if (fDirection == B_HORIZONTAL)
			view->ResizeBy(0,10);
		else
			view->ResizeBy(10,0);
		
		BView::AddChild(view);
	} else {
        /* we need to shrink every View before
        The Divider Index always start´s with 1 and then the index is incrementet by two
        (because between every Divider there is a BView) */
		for (i = 0; i < (fAnz - 2); i++)
			((Divider *)ChildAt((i * 2) + 1))->SetLocation((i + 1) * fStepWidth);
		
		BView::AddChild(new Divider(ChildAt(CountChildren() - 1),view,fDirection));
		((Divider *)ChildAt(CountChildren() - 1))->SetLocation((fAnz - 1) * fStepWidth);
		BView::AddChild(view);
	}
}


bool 
SplitterView::ForceRemove(int32 i)
{
	bool ok = false;
	float d;
	
	// calculate how much space should added to every View  
	if (fDirection == B_HORIZONTAL)
		d = ChildAt(i * 2)->Bounds().Height() / fAnz;
	else
		d = ChildAt(i * 2)->Bounds().Width() / fAnz;
	
    // delete Diverder and View
	ok = BView::RemoveChild(ChildAt(i * 2)); //test obs geklappt hat
	
	// because of the RemoveChild before the Divider is now on the same Index...
	if (ok)
		ok = BView::RemoveChild(ChildAt(i * 2));
	
	// if it was deletet sucsessfully then we add the free space to the remaining Views
	if (ok) {
		// first of all we have one View less
		fAnz--;

		for (uint q = 0;q < fAnz;q++) {
			//took a View and give him his portion of more space
			if (fDirection == B_HORIZONTAL)
				ChildAt(q * 2)->ResizeBy(0,d);
			else
				ChildAt(q * 2)->ResizeBy(d,0);
		}

		/* now we need to update the Divider we use a "tick"
           SetDirection is doing the job for us.. ;-) */
		for (uint32 i = 0; i < (fAnz - 1); i++) 
			((Divider *)ChildAt((i * 2) + 1))->SetDirection(fDirection);
	}
	return ok;
}


bool 
SplitterView::RemoveChild(BView *view)
{
	bool ok = false,found = false;
	uint i = 0;

	// search the Index of the View wich we need to delete
	while ((!found) && (i < fAnz))
	{
		found = (ChildAt(i * 2) == view);
		i++;
	}

	// if we have found the Index
	if (found) {
		// little correction of the i index because we always increased the nuber by one at the end of the loop
		i--;
		// check if the View is the last or the first
		if ((i > 1) && (i < (fAnz - 1))) {
			((Divider *)ChildAt((i * 2) - 1))->SetSecondView(ChildAt((i * 2) + 2));
			ok = ForceRemove(i);
		}
		else{
			// we only need a adjustment if there is more than one View
			if (fAnz > 1) {

				if (i == 0) {
					ChildAt((i * 2) + 2)->MoveTo(0,0);
					ok = ForceRemove(i);
				}
				else{
                    /* little trick ... if we delete the last we also need to delete the Divider before.
                    our ForceRemove deletes always i, and the View i+1 so we only need too swap Divider and View to delete
                    all the Stuff with ForceRemove(index) saves a lot of trouble and work :)
                    */
					BView *tmp1 = ChildAt((i * 2) - 2);
					BView *tmp2 = ChildAt((i * 2) - 1);
					BView::RemoveChild(tmp1);
					BView::RemoveChild(tmp2);
					BView::AddChild(tmp2);
					BView::AddChild(tmp1);
					ok = ForceRemove(i - 1);
				}
			}
			else
				// if there is only one view left we only need to delete this view
				BView::RemoveChild(view);
			}
	}
	return ok;
}


BView *
SplitterView::ViewAt(const uint32 &index)
{
	return (index < fAnz) ? ChildAt(index * 2) : NULL;
}


orientation
SplitterView::GetDirection(void) const
{
	return fDirection;
}


void 
SplitterView::SetDivider(const uint32 &index,const float &location)
{
	if (index < (fAnz - 1))
		((Divider *)ChildAt((index * 2) + 1))->SetLocation(location);
}


void
SplitterView::SetDirection(const orientation &dir)
{
	uint i;
	BView *tmpView;
	float px,py;
	// recalculate every View to the correspondending Size and Position
	if (fDirection != dir) {
		fDirection = dir;
		
		if (fDirection == B_HORIZONTAL) {
			for (i = 0; i < fAnz; i++) {
				tmpView = ChildAt(i * 2);
				// calculate the relative Position from the x position
				px = tmpView->Frame().left / Bounds().Width();
				// convert the relative Postition into a y position
				py = Bounds().Height() * px;
				// move the view to this position
				tmpView->MoveTo(0,py);
				// calculate the relative width
				px = tmpView->Frame().Width() / Bounds().Width();
				// recalculate the new height and set this height
				py = Bounds().Height() * px;
				tmpView->ResizeTo(Bounds().Width(),py);
			}
		} else {
			// explanation see above
			for (i = 0; i < fAnz; i++) {
				tmpView = ChildAt(i * 2);
				py = tmpView->Frame().top / Bounds().Height();
				px = Bounds().Width() * py;
				tmpView->MoveTo(px,0);
				py = tmpView->Frame().Height() / Bounds().Height();
				px = Bounds().Width() * py;
				tmpView->ResizeTo(px,Bounds().Height());
			}
		}
		// tell the Divider that the direction has changed
		for (i = 0; i < (fAnz - 1); i++)
			((Divider *)ChildAt((i * 2) + 1))->SetDirection(fDirection);
	}
}
