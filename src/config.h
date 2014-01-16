#ifndef CONFIG_H
#define CONFIG_H

//Check for new weather every X minutes
#define FREQUENCY_MINUTES 1

//frames
#define TIME_FRAME          (GRect(0, 78, 144, 54))

#define TIME_HOUR_FRAME     (GRect(0, 78, 65, 54))
#define TIME_SEP_FRAME      (GRect(66, 72, 13, 54))
#define TIME_MIN_FRAME      (GRect(79, 78, 66, 54))

#define DATE_FRAME          (GRect(0, 132, 144, 30))
#define WEATHER_ICON_FRAME  (GRect(9, 2, 50, 50))
#define WEATHER_TEMP_FRAME  (GRect(61, 4, 83, 50))
#define WEATHER_HL_FRAME    (GRect(0, 53, 144, 30))
#define INVERT_TOP_FRAME    (GRect(0, 0, 144, 81))
#define INVERT_BOTTOM_FRAME (GRect(0, 81, 144, 87))

#endif
