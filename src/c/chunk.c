#include <pebble.h>
#include <time.h>
#include <ctype.h>

#include "chunk.h"
#include "config.h"
#include "color.h"

//25 up
//15 down

static Window *s_window;
static Layer *s_window_layer, *s_background_layer, *s_time_layer;
static TextLayer *s_date_layer, *s_time_hour_layer, *s_time_separator_layer,
                 *s_time_minutes_layer, *s_temperature_layer, *s_high_low_layer;
static BitmapLayer *s_weather_icon_layer, *s_battery_image_layer;

static GBitmap *s_battery_image, *s_weather_icon;
static char s_temperature_text[8], s_high_low_text[36];
static GFont s_time_font, s_temperature_font;

static int s_timer_minute = 0;
static int s_initial_minute;

// A struct for our specific settings
ClaySettings settings;

static uint8_t BATTERY_ICONS[] = {
  RESOURCE_ID_BATTERY_100,
  RESOURCE_ID_BATTERY_080,
  RESOURCE_ID_BATTERY_060,
  RESOURCE_ID_BATTERY_040,
  RESOURCE_ID_BATTERY_020,
  RESOURCE_ID_BATTERY_000
};

static uint8_t WEATHER_ICONS[] = {
  RESOURCE_ID_IMAGE_TORNADO,
  RESOURCE_ID_IMAGE_TROPICAL_STORM,
  RESOURCE_ID_IMAGE_HURRICANE,
  RESOURCE_ID_IMAGE_SEVERE_THUNDERSTORMS,
  RESOURCE_ID_IMAGE_THUNDERSTORMS,
  RESOURCE_ID_IMAGE_MIXED_RAIN_AND_SNOW,
  RESOURCE_ID_IMAGE_MIXED_RAIN_AND_SLEET,
  RESOURCE_ID_IMAGE_MIXED_SNOW_AND_SLEET,
  RESOURCE_ID_IMAGE_FREEZING_DRIZZLE,
  RESOURCE_ID_IMAGE_DRIZZLE,
  RESOURCE_ID_IMAGE_FREEZING_RAIN,
  RESOURCE_ID_IMAGE_SHOWERS,
  RESOURCE_ID_IMAGE_SHOWERS2,
  RESOURCE_ID_IMAGE_SNOW_FLURRIES,
  RESOURCE_ID_IMAGE_LIGHT_SNOW_SHOWERS,
  RESOURCE_ID_IMAGE_BLOWING_SNOW,
  RESOURCE_ID_IMAGE_SNOW,
  RESOURCE_ID_IMAGE_HAIL,
  RESOURCE_ID_IMAGE_SLEET,
  RESOURCE_ID_IMAGE_DUST,
  RESOURCE_ID_IMAGE_FOGGY,
  RESOURCE_ID_IMAGE_HAZE,
  RESOURCE_ID_IMAGE_SMOKY,
  RESOURCE_ID_IMAGE_BLUSTERY,
  RESOURCE_ID_IMAGE_WINDY,
  RESOURCE_ID_IMAGE_COLD,
  RESOURCE_ID_IMAGE_CLOUDY,
  RESOURCE_ID_IMAGE_MOSTLY_CLOUDY_NIGHT,
  RESOURCE_ID_IMAGE_MOSTLY_CLOUDY_DAY,
  RESOURCE_ID_IMAGE_PARTLY_CLOUDY_NIGHT,
  RESOURCE_ID_IMAGE_PARTLY_CLOUDY_DAY,
  RESOURCE_ID_IMAGE_CLEAR_NIGHT,
  RESOURCE_ID_IMAGE_SUNNY,
  RESOURCE_ID_IMAGE_FAIR_NIGHT,
  RESOURCE_ID_IMAGE_FAIR_DAY,
  RESOURCE_ID_IMAGE_MIXED_RAIN_AND_HAIL,
  RESOURCE_ID_IMAGE_HOT,
  RESOURCE_ID_IMAGE_ISOLATED_THUNDERSTORMS,
  RESOURCE_ID_IMAGE_SCATTERED_THUNDERSTORMS,
  RESOURCE_ID_IMAGE_SCATTERED_THUNDERSTORMS2,
  RESOURCE_ID_IMAGE_SCATTERED_SHOWERS,
  RESOURCE_ID_IMAGE_HEAVY_SNOW,
  RESOURCE_ID_IMAGE_SCATTERED_SNOW_SHOWERS,
  RESOURCE_ID_IMAGE_HEAVY_SNOW2,
  RESOURCE_ID_IMAGE_PARTLY_CLOUDY,
  RESOURCE_ID_IMAGE_THUNDERSHOWERS,
  RESOURCE_ID_IMAGE_SNOW_SHOWERS,
  RESOURCE_ID_IMAGE_ISOLATED_THUNDERSHOWERS,
  RESOURCE_ID_IMAGE_NOT_AVAILABLE
};

typedef enum {
  WEATHER_ICON_TORNADO=0,
  WEATHER_ICON_TROPICAL_STORM=1,
  WEATHER_ICON_HURRICANE=2,
  WEATHER_ICON_SEVERE_THUNDERSTORMS=3,
  WEATHER_ICON_THUNDERSTORMS=4,
  WEATHER_ICON_MIXED_RAIN_AND_SNOW=5,
  WEATHER_ICON_MIXED_RAIN_AND_SLEET=6,
  WEATHER_ICON_MIXED_SNOW_AND_SLEET=7,
  WEATHER_ICON_FREEZING_DRIZZLE=8,
  WEATHER_ICON_DRIZZLE=9,
  WEATHER_ICON_FREEZING_RAIN=10,
  WEATHER_ICON_SHOWERS=11,
  WEATHER_ICON_SHOWERS2=12,
  WEATHER_ICON_SNOW_FLURRIES=13,
  WEATHER_ICON_LIGHT_SNOW_SHOWERS=14,
  WEATHER_ICON_BLOWING_SNOW=15,
  WEATHER_ICON_SNOW=16,
  WEATHER_ICON_HAIL=17,
  WEATHER_ICON_SLEET=18,
  WEATHER_ICON_DUST=19,
  WEATHER_ICON_FOGGY=20,
  WEATHER_ICON_HAZE=21,
  WEATHER_ICON_SMOKY=22,
  WEATHER_ICON_BLUSTERY=23,
  WEATHER_ICON_WINDY=24,
  WEATHER_ICON_COLD=25,
  WEATHER_ICON_CLOUDY=26,
  WEATHER_ICON_MOSTLY_CLOUDY_NIGHT=27,
  WEATHER_ICON_MOSTLY_CLOUDY_DAY=28,
  WEATHER_ICON_PARTLY_CLOUDY_NIGHT=29,
  WEATHER_ICON_PARTLY_CLOUDY_DAY=30,
  WEATHER_ICON_CLEAR_NIGHT=31,
  WEATHER_ICON_SUNNY=32,
  WEATHER_ICON_FAIR_NIGHT=33,
  WEATHER_ICON_FAIR_DAY=34,
  WEATHER_ICON_MIXED_RAIN_AND_HAIL=35,
  WEATHER_ICON_HOT=36,
  WEATHER_ICON_ISOLATED_THUNDERSTORMS=37,
  WEATHER_ICON_SCATTERED_THUNDERSTORMS=38,
  WEATHER_ICON_SCATTERED_THUNDERSTORMS2=39,
  WEATHER_ICON_SCATTERED_SHOWERS=40,
  WEATHER_ICON_HEAVY_SNOW=41,
  WEATHER_ICON_SCATTERED_SNOW_SHOWERS=42,
  WEATHER_ICON_HEAVY_SNOW2=43,
  WEATHER_ICON_PARTLY_CLOUDY=44,
  WEATHER_ICON_THUNDERSHOWERS=45,
  WEATHER_ICON_SNOW_SHOWERS=46,
  WEATHER_ICON_ISOLATED_THUNDERSHOWERS=47,
  WEATHER_ICON_NOT_AVAILABLE=48
} WeatherIcon;


char *upcase(char *str){
  char *s = str;
  while (*s) {
    *s++ = toupper((int)*s);
  }
  return str;
}

const char *getDaySuffix(int day) {
  char *sys_locale = setlocale(LC_ALL, "");
  if (strcmp("en_US", sys_locale) == 0) {
    if (day%100 > 10 && day%100 < 14)
        return "th";
    switch (day%10) {
        case 1:  return "st";
        case 2:  return "nd";
        case 3:  return "rd";
        default: return "th";
    };
  } else {
    return "";
  }
}

static void set_container_image(GBitmap **bmp_image, BitmapLayer *bmp_layer, const int resource_id, GPoint origin) {
  gbitmap_destroy(*bmp_image);
  *bmp_image = gbitmap_create_with_resource(resource_id);
  GRect bounds = gbitmap_get_bounds(*bmp_image);
  GRect frame = (GRect) {
    .origin = origin,
    .size = bounds.size
  };
  bitmap_layer_set_bitmap(bmp_layer, *bmp_image);
  layer_set_frame(bitmap_layer_get_layer(bmp_layer), frame);
}

static bool s_screen_is_obstructed;
static int s_obstruction_diff = 0;

// Event fires once, before the obstruction appears or disappears
void prv_unobstructed_will_change(GRect final_unobstructed_screen_area, void *context) {
  if (s_screen_is_obstructed) {
    // Obstruction is about disappear
  } else {
    // Obstruction is about to appear
    // Hide the things HERE

  }
}

// Event fires once, after obstruction appears or disappears
void prv_unobstructed_did_change(void *context) {
  // Keep track if the screen is obstructed or not
  s_screen_is_obstructed = !s_screen_is_obstructed;

  if(!s_screen_is_obstructed) {
    // Show the things HERE

  }
}

// Event fires frequently, while obstruction is appearing or disappearing
void prv_unobstructed_change(AnimationProgress progress, void *context) {
  // Current unobstructed window size
  GRect full_bounds = layer_get_bounds(s_window_layer);
  GRect bounds = layer_get_unobstructed_bounds(s_window_layer);
  s_obstruction_diff = (full_bounds.size.h - bounds.size.h) / 2.5;

  // Move things HERE
  GRect frame = layer_get_frame(s_background_layer);
  frame.origin.y = s_obstruction_diff * -1;
  layer_set_frame(s_background_layer, frame);

  GRect frame2 = layer_get_frame(text_layer_get_layer(s_temperature_layer));
  frame2.origin.y = WEATHER_TEMP_FRAME.origin.y + (s_obstruction_diff / 2);
  layer_set_frame(text_layer_get_layer(s_temperature_layer), frame2);

  GRect frame3 = layer_get_frame(bitmap_layer_get_layer(s_weather_icon_layer));
  frame3.origin.y = WEATHER_ICON_FRAME.origin.y + (s_obstruction_diff / 2);
  layer_set_frame(bitmap_layer_get_layer(s_weather_icon_layer), frame3);

}

// Initialize the default settings
static void prv_default_settings() {
  settings.TopBackgroundColor = GColorBlack;
  settings.TopForegroundColor = GColorWhite;
  settings.BottomBackgroundColor = GColorWhite;
  settings.BottomForegroundColor = GColorBlack;
  settings.BluetoothVibe = true;
  settings.HourlyVibe = true;
  settings.Blink = false;
  settings.DateFormat = false;
  settings.WeatherIsFahrenheit = true;

  settings.WeatherTemperature = 999;
  settings.WeatherHigh = 999;
  settings.WeatherLow = 999;
  settings.WeatherIcon = 48;
}

// Read settings from persistent storage
static void prv_load_settings() {
  prv_default_settings();
  persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
}

// Save the settings to persistent storage
static void prv_save_settings() {
  persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
}


void weather_set_icon(WeatherIcon icon) {
  set_container_image(&s_weather_icon, s_weather_icon_layer, WEATHER_ICONS[icon], GPoint(ICON_X, ICON_Y + (s_obstruction_diff / 2)));

  if (!gcolor_equal(settings.TopBackgroundColor, GColorWhite)) {
    swap_gbitmap_color(GColorWhite, settings.TopBackgroundColor, s_weather_icon, s_weather_icon_layer);
  }
/*
  gbitmap_destroy(s_weather_icon);
  s_weather_icon = gbitmap_create_with_resource(WEATHER_ICONS[icon]);
  bitmap_layer_set_compositing_mode(s_weather_icon_layer, GCompOpSet);
  bitmap_layer_set_bitmap(s_weather_icon_layer, s_weather_icon);
*/
}

void  prv_set_colors() {
  // TIME HOUR LAYER //
  text_layer_set_background_color(s_time_hour_layer, GColorClear);
  text_layer_set_text_color(s_time_hour_layer, settings.BottomForegroundColor);

  // TIME SEPARATOR LAYER //
  text_layer_set_background_color(s_time_separator_layer, GColorClear);
  text_layer_set_text_color(s_time_separator_layer, settings.BottomForegroundColor);

  // TIME MINUTES LAYER //
  text_layer_set_background_color(s_time_minutes_layer, GColorClear);
  text_layer_set_text_color(s_time_minutes_layer, settings.BottomForegroundColor);

  // DATE LAYER //
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, settings.BottomForegroundColor);

  // TEMPERATURE //
  text_layer_set_background_color(s_temperature_layer, GColorClear);
  text_layer_set_text_color(s_temperature_layer, settings.TopForegroundColor);

  // HIGHLOW //
  text_layer_set_background_color(s_high_low_layer, GColorClear);
  text_layer_set_text_color(s_high_low_layer, settings.TopForegroundColor);

  // ICONS //
  weather_set_icon(settings.WeatherIcon);
}


void weather_set_temperature(int16_t t) {
  if (t == 999) {
    snprintf(s_temperature_text, sizeof(s_temperature_text), "%s\u00B0", "???");
  } else {
    snprintf(s_temperature_text, sizeof(s_temperature_text), "%d\u00B0", t);
  }
  text_layer_set_text(s_temperature_layer, s_temperature_text);
}

void weather_set_loading() {
  snprintf(s_high_low_text, sizeof(s_high_low_text), "%s", "CHUNK v4.0");
  text_layer_set_text(s_high_low_layer, s_high_low_text);
  weather_set_icon(48);
  weather_set_temperature(999);
}

void weather_set_highlow(int16_t high, int16_t low) {
  char *sys_locale = setlocale(LC_ALL, "");

  char wordlow[4] = "MIN";
  char wordhigh[5] = "MAX";

  if (strcmp("en_US", sys_locale) == 0) {
    snprintf(wordlow, sizeof(wordlow), "%s", "LOW");
    snprintf(wordhigh, sizeof(wordhigh), "%s", "HIGH");
  }

  if (high==99 && low==0) {
    snprintf(s_high_low_text, sizeof(s_high_low_text), "%s %s\u00B0 %s %s\u00B0", wordlow, "?", wordhigh, "?");
  } else {
    snprintf(s_high_low_text, sizeof(s_high_low_text), "%s %d\u00B0 %s %d\u00B0", wordlow, low, wordhigh, high);
  }

  text_layer_set_text(s_high_low_layer, s_high_low_text);
}

// HORIZONTAL LINE //
void update_background_callback(Layer *me, GContext* ctx) {

  graphics_context_set_fill_color(ctx, settings.TopBackgroundColor);
  graphics_fill_rect(ctx, INVERT_TOP_FRAME, 0, GCornerNone);

  graphics_context_set_fill_color(ctx, settings.BottomBackgroundColor);
  graphics_fill_rect(ctx, INVERT_BOTTOM_FRAME, 0, GCornerNone);

  graphics_context_set_stroke_color(ctx, settings.BottomForegroundColor);
  graphics_draw_line(ctx, GPoint(0, 83), GPoint(180, 83));
}

static void fetch_data(void);

static void handle_tick(struct tm *tick_time, TimeUnits units_changed) {

  if (units_changed & DAY_UNIT) {

    setlocale(LC_ALL, "");

    static char date_day[4];
    static char date_monthday[3];
    static char date_month[6];
    static char full_date_text[20];

    strftime(date_day, sizeof(date_day), "%a", tick_time);

    strftime(date_monthday,
             sizeof(date_monthday),
             "%d",
             tick_time);

    if (date_monthday[0] == '0') {
      memmove(&date_monthday[0],
              &date_monthday[1],
              sizeof(date_monthday) - 1); //remove leading zero
    }

    strftime(date_month,
             sizeof(date_month),
             "%b",
             tick_time);

    char date_test[50];
    strftime(date_test, sizeof(date_test), "%a %b", tick_time);

    if (settings.DateFormat) {
      snprintf(full_date_text,
                sizeof(full_date_text),
                "%s %s%s %s",
                upcase(date_day),
                date_monthday,
                getDaySuffix(tick_time->tm_mday),
                upcase(date_month));
    } else {
      snprintf(full_date_text,
                sizeof(full_date_text),
                "%s %s %s",
                upcase(date_day),
                date_monthday,
                upcase(date_month));
    }
    text_layer_set_text(s_date_layer, full_date_text);
  }

  if (units_changed & HOUR_UNIT) {
    static char hour_text[] = "00";

    if (settings.HourlyVibe) {
      //vibe!
      vibes_short_pulse();
    }
    /*if(mConfigStyle==5) {
      if(tick_time->tm_hour<6 || tick_time->tm_hour>17) {
        //nightMode();
      }
    }*/
    if (clock_is_24h_style()) {
      strftime(hour_text, sizeof(hour_text), "%H", tick_time);
    } else {
      strftime(hour_text, sizeof(hour_text), "%I", tick_time);
      if (hour_text[0] == '0') {
        layer_set_frame(s_time_layer, GRect(-13, 0, 144, 168)); //shift time left to centralise it
        memmove(&hour_text[0], &hour_text[1], sizeof(hour_text) - 1); //remove leading zero
      }
      else {
        layer_set_frame(s_time_layer, GRect(0, 0, 144, 168));
      }
    }
    text_layer_set_text(s_time_hour_layer, hour_text);
  }
  if (units_changed & MINUTE_UNIT) {
    static char minute_text[] = "00";
    strftime(minute_text, sizeof(minute_text), "%M", tick_time);
    text_layer_set_text(s_time_minutes_layer, minute_text);

    if (FREQUENCY_MINUTES == s_timer_minute) {
      fetch_data();
      s_timer_minute = 0;
    } else {
      s_timer_minute++;
    }

  }
  if (settings.Blink && (units_changed & SECOND_UNIT)) {
    layer_set_hidden(text_layer_get_layer(s_time_separator_layer), tick_time->tm_sec%2);
  }
}


static void in_received_handler(DictionaryIterator *iter, void *context) {
  bool setHighLow = false;

  Tuple *ready_tuple = dict_find(iter, MESSAGE_KEY_JSReady);
  if(ready_tuple) {
    fetch_data();
    return;
  }

  // Top Background Color
  Tuple *tbg_color_t = dict_find(iter, MESSAGE_KEY_TopBackgroundColor);
  if (tbg_color_t) {
    settings.TopBackgroundColor = GColorFromHEX(tbg_color_t->value->int32);
  }

  // Top Foreground Color
  Tuple *tfg_color_t = dict_find(iter, MESSAGE_KEY_TopForegroundColor);
  if (tfg_color_t) {
    settings.TopForegroundColor = GColorFromHEX(tfg_color_t->value->int32);
  }

  // Bottom Background Color
  Tuple *bbg_color_t = dict_find(iter, MESSAGE_KEY_BottomBackgroundColor);
  if (bbg_color_t) {
    settings.BottomBackgroundColor = GColorFromHEX(bbg_color_t->value->int32);
  }

  // Bottom Foreground Color
  Tuple *bfg_color_t = dict_find(iter, MESSAGE_KEY_BottomForegroundColor);
  if (bfg_color_t) {
    settings.BottomForegroundColor = GColorFromHEX(bfg_color_t->value->int32);
  }

  prv_set_colors();

  // Bluetooth Vibe
  Tuple *bluetooth_vibe_t = dict_find(iter, MESSAGE_KEY_BluetoothVibe);
  if (bluetooth_vibe_t) {
    settings.BluetoothVibe = bluetooth_vibe_t->value->int32 == 1;
  }

  // Hourly Vibe
  Tuple *hourly_vibe_t = dict_find(iter, MESSAGE_KEY_HourlyVibe);
  if (hourly_vibe_t) {
    settings.HourlyVibe = hourly_vibe_t->value->int32 == 1;
  }

  // Blink
  Tuple *blink_t = dict_find(iter, MESSAGE_KEY_Blink);
  if (blink_t) {
    bool Blink = blink_t->value->int32 == 1;
    if (settings.Blink != Blink) {
      settings.Blink = Blink;
      tick_timer_service_unsubscribe();
      if (settings.Blink) {
        tick_timer_service_subscribe(SECOND_UNIT, handle_tick);
      } else {
        tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
      }
    }
  }

  // Date Format
  Tuple *date_format_t = dict_find(iter, MESSAGE_KEY_DateFormat);
  if (date_format_t) {
    settings.DateFormat = date_format_t->value->int32 == 1;
    time_t now = time(NULL);
    struct tm *tick_time = localtime(&now);
    handle_tick(tick_time, DAY_UNIT);
  }

  // Weather Is Fahrenheit
  Tuple *weather_is_fahrenheit_t = dict_find(iter, MESSAGE_KEY_WeatherIsFahrenheit);
  if (weather_is_fahrenheit_t) {
    bool WeatherIsFahrenheit = weather_is_fahrenheit_t->value->int32 == 1;
    if (settings.WeatherIsFahrenheit != WeatherIsFahrenheit) {
      settings.WeatherIsFahrenheit = WeatherIsFahrenheit;
      fetch_data();
      return;
    }
  }

  Tuple *weather_temperature_tuple = dict_find(iter, MESSAGE_KEY_WeatherTemperature);
  if (weather_temperature_tuple) {
    settings.WeatherTemperature = weather_temperature_tuple->value->int16;
    weather_set_temperature(settings.WeatherTemperature);
  }

  Tuple *weather_high_tuple = dict_find(iter, MESSAGE_KEY_WeatherHigh);
  if (weather_high_tuple) {
    settings.WeatherHigh = weather_high_tuple->value->int16;
    setHighLow = true;
  }

  Tuple *weather_low_tuple = dict_find(iter, MESSAGE_KEY_WeatherLow);
  if (weather_low_tuple) {
    settings.WeatherLow = weather_low_tuple->value->int16;
    setHighLow = true;
  }

  Tuple *weather_icon_tuple = dict_find(iter, MESSAGE_KEY_WeatherIcon);
  if (weather_icon_tuple) {
    settings.WeatherIcon = weather_icon_tuple->value->uint8;
    weather_set_icon(settings.WeatherIcon);
  }

  if (setHighLow) {
    weather_set_highlow(settings.WeatherHigh, settings.WeatherLow);
  }

  // Save the new settings to persistent storage
  prv_save_settings();

}

static void toggle_bluetooth(bool connected) {
  if (settings.BluetoothVibe) {
    if (!connected) {
      vibes_long_pulse();
    } else {
      vibes_double_pulse();
    }
  }
}
void bluetooth_connection_callback(bool connected) {
  toggle_bluetooth(connected);
}

static void fetch_data(void) {
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  if (iter == NULL) {
    return;
  }
  app_message_outbox_send();
}

static void app_message_init(void) {
  app_message_register_inbox_received(in_received_handler);
  app_message_open(128, 128);
}

static void update_battery(BatteryChargeState charge_state) {
  uint8_t batteryPercent;
  uint8_t img;

  batteryPercent = charge_state.charge_percent;

  if (batteryPercent>=90) {
     img = 0;
  } else if (batteryPercent>=80) {
     img = 1;
  } else if (batteryPercent>=60) {
     img = 2;
  } else if (batteryPercent>=40) {
     img = 3;
  } else if (batteryPercent>=20) {
     img = 4;
  } else {
     img = 5;
  }
  set_container_image(&s_battery_image, s_battery_image_layer, BATTERY_ICONS[img], GPoint(BATTERY_X, 82));

  if(!gcolor_equal(settings.BottomBackgroundColor, GColorBlack)) {
    swap_gbitmap_color(GColorWhite, settings.BottomBackgroundColor, s_battery_image, s_battery_image_layer);
    swap_gbitmap_color(GColorBlack, settings.TopBackgroundColor, s_battery_image, s_battery_image_layer);
  }
}

void handle_init(void) {

  prv_load_settings();

  app_message_init();

  // WINDOW //
  s_window = window_create();
  if (s_window == NULL) {
      return;
  }
  window_stack_push(s_window, true /* Animated */);
  s_window_layer = window_get_root_layer(s_window);
  window_set_background_color(s_window, GColorBlack);
  //layer_set_update_proc(s_window_layer, update_background_callback);

  // BACKGROUND //
  s_background_layer = layer_create(layer_get_frame(s_window_layer));
  layer_add_child(s_window_layer, s_background_layer);
  layer_set_update_proc(s_background_layer, update_background_callback);

  //BATTERY_ICONS
  s_battery_image = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_100);
  GRect bounds = gbitmap_get_bounds(s_battery_image);

  GRect frame4 = (GRect) {
    .origin = { .x = BATTERY_X, .y = 82 },
    .size = bounds.size
  };

  s_battery_image_layer = bitmap_layer_create(frame4);
  bitmap_layer_set_bitmap(s_battery_image_layer, s_battery_image);
  layer_add_child(s_background_layer, bitmap_layer_get_layer(s_battery_image_layer));

  // FONTS //
  ResHandle res_t = resource_get_handle(RESOURCE_ID_BIG_52);
  ResHandle res_temp = resource_get_handle(RESOURCE_ID_MEDIUM_34);

  s_time_font = fonts_load_custom_font(res_t);
  s_temperature_font = fonts_load_custom_font(res_temp);

  // TIME LAYER //
  s_time_layer = layer_create(layer_get_frame(s_window_layer));
  layer_add_child(s_background_layer, s_time_layer);

  // TIME HOUR LAYER //
  s_time_hour_layer = text_layer_create(TIME_HOUR_FRAME);
  text_layer_set_background_color(s_time_hour_layer, settings.BottomBackgroundColor);
  text_layer_set_text_color(s_time_hour_layer, settings.BottomForegroundColor);
  text_layer_set_font(s_time_hour_layer, s_time_font);
  text_layer_set_text_alignment(s_time_hour_layer, GTextAlignmentRight);
  layer_add_child(s_background_layer, text_layer_get_layer(s_time_hour_layer));

  // TIME SEPARATOR LAYER //
  s_time_separator_layer = text_layer_create(TIME_SEP_FRAME);
  text_layer_set_background_color(s_time_separator_layer, settings.BottomBackgroundColor);
  text_layer_set_text_color(s_time_separator_layer, settings.BottomForegroundColor);
  text_layer_set_font(s_time_separator_layer, s_time_font);
  text_layer_set_text_alignment(s_time_separator_layer, GTextAlignmentCenter);
  text_layer_set_text(s_time_separator_layer, ":");
  layer_add_child(s_background_layer, text_layer_get_layer(s_time_separator_layer));

  // TIME MINUTES LAYER //
  s_time_minutes_layer = text_layer_create(TIME_MIN_FRAME);
  text_layer_set_background_color(s_time_minutes_layer, settings.BottomBackgroundColor);
  text_layer_set_text_color(s_time_minutes_layer, settings.BottomForegroundColor);
  text_layer_set_font(s_time_minutes_layer, s_time_font);
  text_layer_set_text_alignment(s_time_minutes_layer, GTextAlignmentLeft);
  layer_add_child(s_background_layer, text_layer_get_layer(s_time_minutes_layer));

  // DATE LAYER //
  s_date_layer = text_layer_create(DATE_FRAME);
  text_layer_set_background_color(s_date_layer, settings.BottomBackgroundColor);
  text_layer_set_text_color(s_date_layer, settings.BottomForegroundColor);
  text_layer_set_font(s_date_layer, fonts_get_system_font(PBL_IF_RECT_ELSE(FONT_KEY_GOTHIC_28_BOLD, FONT_KEY_GOTHIC_24_BOLD)));//mDateFont);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  layer_add_child(s_background_layer, text_layer_get_layer(s_date_layer));

  // WEATHER ICON //
  s_weather_icon_layer = bitmap_layer_create(WEATHER_ICON_FRAME);
  //bitmap_layer_set_compositing_mode(s_weather_icon_layer, GCompOpSet);
  layer_add_child(s_background_layer, bitmap_layer_get_layer(s_weather_icon_layer));

  // TEMPERATURE //
  s_temperature_layer = text_layer_create(WEATHER_TEMP_FRAME);
  text_layer_set_background_color(s_temperature_layer, settings.TopBackgroundColor);
  text_layer_set_text_color(s_temperature_layer, settings.TopForegroundColor);
  text_layer_set_font(s_temperature_layer, s_temperature_font); //fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD)); //
  text_layer_set_text_alignment(s_temperature_layer, GTextAlignmentCenter);
  layer_add_child(s_background_layer, text_layer_get_layer(s_temperature_layer));

  // HIGHLOW //
  s_high_low_layer = text_layer_create(WEATHER_HL_FRAME);
  text_layer_set_background_color(s_high_low_layer, settings.TopBackgroundColor);
  text_layer_set_text_color(s_high_low_layer, settings.TopForegroundColor);
  text_layer_set_font(s_high_low_layer, fonts_get_system_font(PBL_IF_RECT_ELSE(FONT_KEY_GOTHIC_18_BOLD, FONT_KEY_GOTHIC_14_BOLD))); //mHighLowFont);
  text_layer_set_text_alignment(s_high_low_layer, GTextAlignmentCenter);
  layer_add_child(s_background_layer, text_layer_get_layer(s_high_low_layer));

  prv_set_colors();

  weather_set_loading();

  time_t now = time(NULL);
  struct tm *tick_time = localtime(&now);

  s_initial_minute = (tick_time->tm_min % FREQUENCY_MINUTES);

  handle_tick(tick_time, DAY_UNIT + HOUR_UNIT + MINUTE_UNIT + SECOND_UNIT);

  if (settings.Blink) {
    tick_timer_service_subscribe(SECOND_UNIT, handle_tick);
  } else {
    tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
  }
  bluetooth_connection_service_subscribe(bluetooth_connection_callback);

  update_battery(battery_state_service_peek());
  battery_state_service_subscribe(&update_battery);

  GRect fullscreen = layer_get_bounds(s_window_layer);
  GRect unobstructed_bounds = layer_get_unobstructed_bounds(s_window_layer);

  // Determine if the screen is obstructed when the app starts
  s_screen_is_obstructed = !grect_equal(&fullscreen, &unobstructed_bounds);

  if(s_screen_is_obstructed) {
    // Hide things HERE
  } else {
    // Show things HERE
  }
  // Fire the handler to move things
  prv_unobstructed_change(100, NULL);

  // Subscribe to the unobstructed area events
  UnobstructedAreaHandlers handlers = {
    .will_change = prv_unobstructed_will_change,
    .change = prv_unobstructed_change,
    .did_change = prv_unobstructed_did_change
  };
  unobstructed_area_service_subscribe(handlers, NULL);
}

void handle_deinit(void) {
  fonts_unload_custom_font(s_time_font);
  fonts_unload_custom_font(s_temperature_font);

  layer_remove_from_parent(bitmap_layer_get_layer(s_weather_icon_layer));
  bitmap_layer_destroy(s_weather_icon_layer);
  gbitmap_destroy(s_weather_icon);

  layer_remove_from_parent(bitmap_layer_get_layer(s_battery_image_layer));
  bitmap_layer_destroy(s_battery_image_layer);
  gbitmap_destroy(s_battery_image);
  s_battery_image = NULL;

  tick_timer_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
  battery_state_service_unsubscribe();
  unobstructed_area_service_unsubscribe();

  text_layer_destroy(s_high_low_layer);
  text_layer_destroy(s_time_separator_layer);
  text_layer_destroy(s_temperature_layer);
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_time_minutes_layer);
  text_layer_destroy(s_time_hour_layer);
  layer_destroy(s_time_layer);
  layer_destroy(s_background_layer);

  window_destroy(s_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
