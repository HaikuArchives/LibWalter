//ScreenInfo.cpp
// Autor: Christian Lörchner

#include "ScreenInfo.h"

ScreenInfo::ScreenInfo()
	:BScreen(B_MAIN_SCREEN_ID)
{
  UpdateInfo();
}

bool ScreenInfo::UpdateInfo()
{
  GetMode(&mode);
  x = mode.virtual_width;
  y = mode.virtual_height;
  return true;
}

int32 ScreenInfo::GetX()
{
  return x;
}

int32 ScreenInfo::GetY()
{
  return y;
}
