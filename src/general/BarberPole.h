/*
 * Copyright 1998 dirk116, dirk116@wallace.free.de
 * Copyright 2003 Michael Pfeiffer
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#ifndef BARBERPOLE_H_
#define BARBERPOLE_H_

#include <View.h>

enum {
	BARBERPOLE_FROM_RIGHT = 0,
	BARBERPOLE_FROM_LEFT
};

enum {
	BARBERPOLE_SIMPLE = 0,
	BARBERPOLE_FANCY
};

class BarberPole : public BView
{
public:
					BarberPole(BRect frame, const char *name, uint32 resizingMode,
								uint32 flags, int pDirection = BARBERPOLE_FROM_LEFT);

			void	SetLowColor(const rgb_color &c);
			void	SetHighColor(const rgb_color &c);
	
	virtual void	Draw(BRect rect);
	virtual void	Pulse();
	
			void	SetStyle(int style);
			int		Style(void) const;
	
private:
	static	void	SetColors(rgb_color* colors, const rgb_color &c);
	
	int				fDirection;
	pattern			fStripes;
	
	rgb_color		fHighColors[4];
	rgb_color		fLowColors[4];
	
	int				fStyle;
};

#endif