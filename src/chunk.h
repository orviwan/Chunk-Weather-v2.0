#pragma once
#include <pebble.h>

#define SETTINGS_KEY 1

// A structure containing our settings
typedef struct ClaySettings {
  GColor BackgroundColor;
  GColor ForegroundColor;
  bool SecondTick;
  bool Animations;
} __attribute__((__packed__)) ClaySettings;
