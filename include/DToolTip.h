/*
	DToolTip:	An easy-to-use tooltip control
	Copyright 2009 DarkWyrm <darkwyrm@gmail.com>
	Released under the MIT license
*/
#ifndef DTOOLTIP_H
#define DTOOLTIP_H

#include <Window.h>
#include <View.h>

using namespace std;

/*
	Usage:
	This class makes it about as simple as possible to add bubble help to
	your applications. There is only one DToolTip instance in any application.
	
	1) Call DToolTip::GetInstance() to get a pointer
	2) Call SetTip to add or edit bubble help for a view. Pass NULL as the
		text parameter to remove a view's tooltip
*/

class DTipWindow;

class DToolTip
{
public:
						DToolTip(void);
	virtual				~DToolTip(void);

			void		SetTip(BView *view, char *text);
			void		EnableTips(bool enable = true);
			
			void		SetTipDelay(bigtime_t delay);
			bigtime_t	GetTipDelay(void);
			
	static	DToolTip *	GetInstance(void);
	
private:
			void		MakeValid(void);
			
			DTipWindow	*fTipWindow;
};

#endif
