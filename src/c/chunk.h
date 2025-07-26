#pragma once
#include <pebble.h>

#define SETTINGS_KEY 1

// A structure containing our settings
typedef struct ClaySettings {
  GColor TopBackgroundColor;
  GColor TopForegroundColor;
  GColor BottomBackgroundColor;
  GColor BottomForegroundColor;
  bool BluetoothVibe;
  bool HourlyVibe;
  bool Blink;
  bool DateFormat;
  bool WeatherIsFahrenheit;
  int WeatherTemperature;
  int WeatherHigh;
  int WeatherLow;
  int WeatherIcon;
} __attribute__((__packed__)) ClaySettings;
