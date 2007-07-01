/*
 * Copyright 2007 Matthias Lindner, aka Paradoxon
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#ifndef B_VIEW_SPLITTER_H
#define B_VIEW_SPLITTER_H

#include <View.h>
#include "Divider.h"

class SplitterView : public BView
{
public:
					SplitterView(const BRect &frame, const orientation &dir,
								const uint32 &resizingMode,
								const uint32 &flags);
		void		AddChild(BView *view);
		bool		RemoveChild(BView *view);
		BView *		ViewAt(const uint32 &index);
		void		SetDivider(const uint32 &index, const float &location);
		
		void		SetDirection(const orientation &dir);
		orientation	GetDirection(void) const;
		
private:
        bool		ForceRemove(int32 i);
  		
  		orientation	fDirection;
  		uint32		fAnz;
  		float 		fStepWidth;
};

#endif
