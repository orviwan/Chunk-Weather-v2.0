#pragma once
#include <pebble.h>

//Check for new weather every X minutes
#define FREQUENCY_MINUTES 15

#define BATTERY_X PBL_IF_RECT_ELSE(50, 68)
#define ICON_X PBL_IF_RECT_ELSE(9, 27)
#define ICON_Y PBL_IF_RECT_ELSE(2, 32)

//frames
#define TIME_HOUR_FRAME     PBL_IF_RECT_ELSE(GRect(0, 78, 65, 54), GRect(18, 78, 65, 54))
#define TIME_SEP_FRAME      PBL_IF_RECT_ELSE(GRect(66, 72, 13, 54), GRect(84, 72, 13, 54))
#define TIME_MIN_FRAME      PBL_IF_RECT_ELSE(GRect(79, 78, 66, 54), GRect(97, 78, 66, 54))

#define DATE_FRAME          PBL_IF_RECT_ELSE(GRect(0, 132, 144, 30), GRect(18, 129, 144, 30))
#define WEATHER_ICON_FRAME  PBL_IF_RECT_ELSE(GRect(9, 2, 50, 50), GRect(27, 32, 50, 50))
#define WEATHER_TEMP_FRAME  PBL_IF_RECT_ELSE(GRect(61, 4, 83, 50), GRect(79, 36, 83, 50))
#define WEATHER_HL_FRAME    PBL_IF_RECT_ELSE(GRect(0, 53, 144, 30), GRect(18, 13, 144, 30))
#define INVERT_TOP_FRAME    PBL_IF_RECT_ELSE(GRect(0, 0, 144, 83), GRect(18, 0, 144, 83))
#define INVERT_BOTTOM_FRAME PBL_IF_RECT_ELSE(GRect(0, 83, 144, 87), GRect(18, 83, 144, 87))
