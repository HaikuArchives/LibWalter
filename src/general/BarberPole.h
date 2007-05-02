#ifndef _BARBERPOLE_H_
#define _BARBERPOLE_H_

/*
	BarberPole.h: 
	Written by Michael Pfeiffer, dirk116
	Released under the MIT license.
*/

#include <View.h>

const int FROM_RIGHT_TO_LEFT = 0;
const int FROM_LEFT_TO_RIGHT = 1;

#define BB_LOOK_3D 1
#define BB_NUM_SHADES 4

class BBarberPole : public BView {
public:
	BBarberPole(BRect pRect, const char *pName, uint32 resizingMode, uint32 flags, int pDirection = FROM_LEFT_TO_RIGHT);

#if BB_LOOK_3D
	void SetLowColor(rgb_color c);
	void SetHighColor(rgb_color c);
#endif
	void Draw(BRect rect);
	void Pulse();
	
private:
	static void SetColors(rgb_color* colors, rgb_color c);
	
	int		fDirection;
	pattern fStripes;
#if BB_LOOK_3D
	rgb_color fHighColors[BB_NUM_SHADES];
	rgb_color fLowColors[BB_NUM_SHADES];
#endif
};

#endif