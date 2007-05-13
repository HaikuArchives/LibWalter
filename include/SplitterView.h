/*
 * Copyright 2007 Matthias Lindner, aka Paradoxon
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#ifndef B_VIEW_SPLITTER_H
#define B_VIEW_SPLITTER_H

#include <View.h>
#include "Divider.h"

class BViewSplitter:public BView{
 public:
//					BViewSplitter(BRect frame,uint anz,BView *vws[],orientation dr,uint32 resizingMode,uint32 flags);
					BViewSplitter(BRect frame,orientation dr,uint32 resizingMode,uint32 flags);
		void		AddChild(BView *view);
		bool		RemoveChild(BView *view);
		BView*      GetView(uint32 indx);
		orientation	GetDirection();
		void		SetDivPos(uint32 indx,float location);
		void		SetDirection(orientation dr);
 private:
        bool		ForceRemove(int32 i);
  		orientation	direction;
  		uint32		anz;
  		float 		stepWidth;
};

#endif
