#include <pebble.h>
#include <time.h>
#include <ctype.h>

#include "config.h"

static Window *mWindow;

static Layer *mWindowLayer;
static Layer *mBackgroundLayer;

static BitmapLayer *mWeatherIconLayer;

static InverterLayer *mInvertBottomLayer;
static InverterLayer *mInvertTopLayer;

static TextLayer *mDateLayer;
static Layer *mTimeLayer;
static TextLayer *mTimeHourLayer;
static TextLayer *mTimeSeparatorLayer;
static TextLayer *mTimeMinutesLayer;
static TextLayer *mTemperatureLayer;
static TextLayer *mHighLowLayer;

static char mTemperatureText[8];
static char mHighLowText[36];

static GBitmap *mWeatherIcon;

static GFont *mDateFont;
static GFont *mTimeFont;
static GFont *mTemperatureFont;   
static GFont *mHighLowFont;

//static int mInitialMinute;

static int mConfigStyle;               //1=BlackOnWhite, 2=Split1(WhiteTop), 3=WhiteOnBlack, 4=Split2(BlackTop)
static int mConfigBluetoothVibe;       //0=off 1=on
static int mConfigHourlyVibe;          //0=off 1=on
static int mConfigWeatherUnit;         //1=Celsius 0=Fahrenheit
static int mConfigBlink;               //0=Static 1=Blink
static int mConfigDateFormat;          //0=Default 1=NoSuffix

static int mTemperatureDegrees;        //-999 to 999
static int mTemperatureIcon;           //0 to 48
static int mTemperatureHigh;           //-999 to 999
static int mTemperatureLow;            //-999 to 999


enum {
  STYLE_KEY = 0x0,                     // TUPLE_INT
  BLUETOOTHVIBE_KEY = 0x1,             // TUPLE_INT
  HOURLYVIBE_KEY = 0x2,                // TUPLE_INT
  WEATHER_UNITS = 0x3,                 // TUPLE_INT
  WEATHER_TEMPERATURE_KEY = 0x4,       // TUPLE_INT
  WEATHER_ICON_KEY = 0x5,              // TUPLE_INT
  WEATHER_mTemperatureHigh_KEY = 0x6,  // TUPLE_INT
  WEATHER_mTemperatureLow_KEY = 0x7,   // TUPLE_INT
  BLINK_KEY = 0x8,                     // TUPLE_INT
  DATEFORMAT_KEY = 0x9                 // TUPLE_INT
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
    if (day%100 > 10 && day%100 < 14)
        return "th";
    switch (day%10) {
        case 1:  return "st";
        case 2:  return "nd";
        case 3:  return "rd";
        default: return "th";
    };
}

static void set_container_image(GBitmap **bmp_image, BitmapLayer *bmp_layer, const int resource_id, GPoint origin) {
  GBitmap *old_image = *bmp_image;
  *bmp_image = gbitmap_create_with_resource(resource_id);
  GRect frame = (GRect) {
    .origin = origin,
    .size = (*bmp_image)->bounds.size
  };
  bitmap_layer_set_bitmap(bmp_layer, *bmp_image);
  layer_set_frame(bitmap_layer_get_layer(bmp_layer), frame);
  gbitmap_destroy(old_image);
}

static void remove_invert_top() {
  if(mInvertTopLayer!=NULL) {
    layer_remove_from_parent(inverter_layer_get_layer(mInvertTopLayer));
    inverter_layer_destroy(mInvertTopLayer);
    mInvertTopLayer = NULL;
  }
}

static void remove_invert_bottom() {
  if(mInvertBottomLayer!=NULL) {
    layer_remove_from_parent(inverter_layer_get_layer(mInvertBottomLayer));
    inverter_layer_destroy(mInvertBottomLayer);
    mInvertBottomLayer = NULL;
  }
}

static void set_invert_top() {
  if(!mInvertTopLayer) {
    mInvertTopLayer = inverter_layer_create(INVERT_TOP_FRAME);  
    layer_add_child(mWindowLayer, inverter_layer_get_layer(mInvertTopLayer));
  }
}

static void set_invert_bottom() {
  if(!mInvertBottomLayer) {
    mInvertBottomLayer = inverter_layer_create(INVERT_BOTTOM_FRAME);  
    layer_add_child(mWindowLayer, inverter_layer_get_layer(mInvertBottomLayer));
  }
}

static void setStyle() {
  remove_invert_top();
  remove_invert_bottom();
  switch(mConfigStyle) {
    case 1:
      break;
    case 2:
      set_invert_bottom();
      break;
    case 3:
      set_invert_top();
      set_invert_bottom();
      break;
    case 4:
      set_invert_top();
      break;      
  }
}

void weather_set_icon(WeatherIcon icon) {	
  layer_remove_from_parent(bitmap_layer_get_layer(mWeatherIconLayer));
  mWeatherIconLayer = bitmap_layer_create(WEATHER_ICON_FRAME);  
  set_container_image(&mWeatherIcon, mWeatherIconLayer, WEATHER_ICONS[icon], GPoint(9, 2));  
	layer_add_child(mWindowLayer, bitmap_layer_get_layer(mWeatherIconLayer));
  
  //Reapply inverter
  setStyle();
}

void weather_set_temperature(int16_t t) {
	snprintf(mTemperatureText, sizeof(mTemperatureText), "%d\u00B0", t); //\u00B0
	text_layer_set_text(mTemperatureLayer, mTemperatureText);  
}

void weather_set_loading() {
	snprintf(mHighLowText, sizeof(mHighLowText), "%s", "CHUNK v2.0"); //"LOW 999\u00B0 HIGH 999\u00B0"); //
	text_layer_set_text(mHighLowLayer, mHighLowText);
  weather_set_icon(48);  
  weather_set_temperature(999);
}

void weather_set_highlow(int16_t high, int16_t low) {
	snprintf(mHighLowText, sizeof(mHighLowText), "LOW %d\u00B0 HIGH %d\u00B0", low, high);
	text_layer_set_text(mHighLowLayer, mHighLowText);
}

/*
void weather_set_error(WeatherLayer* weather_layer, int http_status) {
	weather_set_icon(weather_layer, WEATHER_ICON_NOT_AVAILABLE);
	if(http_status==0) {
		snprintf(temperature, sizeof(temperature), "%s%s", "?", "...");
	}
	else {
		snprintf(temperature, sizeof(temperature), "%s%d", "E:", http_status-1000);
	}
	text_layer_set_text(&mTemperatureLayer, temperature);  
}
*/




void request_weather();

/*
void handle_failed(int32_t cookie, int http_status, void* context) {
	if(cookie == 0 || cookie == WEATHER_HTTP_COOKIE) {
		weather_set_error(&weather_layer, http_status);
	}
	link_monitor_handle_failure(http_status);	
	//request the location and weather on next minute tick
	located = false;
}

void handle_success(int32_t cookie, int http_status, DictionaryIterator* received, void* context) {
	if(cookie != WEATHER_HTTP_COOKIE) return;

	//Weather icon
	Tuple* icon_tuple = dict_find(received, WEATHER_CODE);
	if(icon_tuple) {
		icon = icon_tuple->value->int16;
		if(icon >= 0 && icon <= 48) {
			weather_set_icon(&weather_layer, icon);
		} else {
			weather_set_error(&weather_layer, http_status);
		}
	}
	
	//Today's high temperature
	Tuple* high_tuple = dict_find(received, WEATHER_KEY_HIGH);
	if(high_tuple) {
		high = high_tuple->value->int16;
	}
	
	//Today's low temperature
	Tuple* low_tuple = dict_find(received, WEATHER_KEY_LOW);
	if(low_tuple) {
		low = low_tuple->value->int16;
	}
	
	//Current temperature
	Tuple* temperature_tuple = dict_find(received, WEATHER_KEY_TEMPERATURE);
	if(temperature_tuple) {
		weather_set_highlow(&weather_layer, high, low);
		weather_set_temperature(&weather_layer, temperature_tuple->value->int16);
	}

	link_monitor_handle_success();
}

void handle_location(float latitude, float longitude, float altitude, float accuracy, void* context) {
	// Fix the floats
	our_latitude = latitude * 1000000;
	our_longitude = longitude * 1000000;
	located = true;
	request_weather();
}


*/

// HORIZONTAL LINE //
void update_background_callback(Layer *me, GContext* ctx) { 
	graphics_context_set_stroke_color(ctx, GColorBlack);
	graphics_draw_line(ctx, GPoint(0, 81), GPoint(144, 81));		
}





static void handle_tick(struct tm *tick_time, TimeUnits units_changed) {

  if (units_changed & DAY_UNIT) {
  
    static char date_day[] = "XXX";
    static char date_monthday[] = "00";
    static char date_month[] = "XXX";    
    static char full_date_text[20] = "";
    
    strftime(date_day,
               sizeof(date_day),
               "%a",
               tick_time);

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
             
    if(mConfigDateFormat==0) {
      snprintf(full_date_text, 
                sizeof(full_date_text), 
                "%s %s%s %s", 
                upcase(date_day), 
                date_monthday, 
                getDaySuffix(tick_time->tm_mday),
                upcase(date_month)); 
    }
    else {
      snprintf(full_date_text, 
                sizeof(full_date_text), 
                "%s %s %s", 
                upcase(date_day), 
                date_monthday, 
                upcase(date_month)); 
    }
    text_layer_set_text(mDateLayer, full_date_text);
  }
	
    
  if (units_changed & HOUR_UNIT) { 
    static char hour_text[] = "00";
    
    if(mConfigHourlyVibe) {
      //vibe!
      vibes_short_pulse();
    }
    if(mConfigStyle==5) {
      if(tick_time->tm_hour<6 || tick_time->tm_hour>17) {
        //nightMode();
      }
    }    
    if(clock_is_24h_style()) {
      strftime(hour_text, sizeof(hour_text), "%H", tick_time);
    }
    else {
      strftime(hour_text, sizeof(hour_text), "%I", tick_time);
      if (hour_text[0] == '0') {
        layer_set_frame(mTimeLayer, GRect(-13, 0, 144, 168)); //shift time left to centralise it
        memmove(&hour_text[0], &hour_text[1], sizeof(hour_text) - 1); //remove leading zero
      }
      else {
        layer_set_frame(mTimeLayer, GRect(0, 0, 144, 168));
      }
    }
    text_layer_set_text(mTimeHourLayer, hour_text);
  }
  if (units_changed & MINUTE_UNIT) {
    static char minute_text[] = "00";	
    strftime(minute_text, sizeof(minute_text), "%M", tick_time);	
    text_layer_set_text(mTimeMinutesLayer, minute_text);
  }
  if (mConfigBlink && (units_changed & SECOND_UNIT)) {
    layer_set_hidden(text_layer_get_layer(mTimeSeparatorLayer), tick_time->tm_sec%2);
  }


/*
	if(!located || (t->tick_time->tm_min % FREQUENCY_MINUTES) == mInitialMinute) {
		//Every FREQUENCY_MINUTES, request updated weather
		http_location_request(); 
	}
	else {
		//Every minute, ping the phone
		link_monitor_ping();
	}
*/
}
static void in_received_handler(DictionaryIterator *iter, void *context) {
  bool setHighLow = false;
  Tuple *style_tuple = dict_find(iter, STYLE_KEY);
  if (style_tuple && style_tuple->value->uint8 != mConfigStyle) {
    mConfigStyle = style_tuple->value->uint8;
    setStyle();
  }
  Tuple *bluetoothvibe_tuple = dict_find(iter, BLUETOOTHVIBE_KEY);
  if (bluetoothvibe_tuple) {
    mConfigBluetoothVibe = bluetoothvibe_tuple->value->uint8;
  }
  Tuple *hourlyvibe_tuple = dict_find(iter, HOURLYVIBE_KEY);
  if (hourlyvibe_tuple) {
    mConfigHourlyVibe = hourlyvibe_tuple->value->uint8;
  }  
  Tuple *blink_tuple = dict_find(iter, BLINK_KEY);
  if (blink_tuple) {
    mConfigBlink = blink_tuple->value->uint8;
  }
  Tuple *dateformat_tuple = dict_find(iter, DATEFORMAT_KEY);
  if (dateformat_tuple && dateformat_tuple->value->uint8 != mConfigDateFormat) {
    mConfigDateFormat = dateformat_tuple->value->uint8;
    time_t now = time(NULL);
    struct tm *tick_time = localtime(&now);  
    handle_tick(tick_time, DAY_UNIT);
  }  
  Tuple *units_tuple = dict_find(iter, WEATHER_UNITS);
  if (units_tuple) {
    mConfigWeatherUnit = units_tuple->value->uint8;
  }
  Tuple *weather_temperature_tuple = dict_find(iter, WEATHER_TEMPERATURE_KEY);
  if (weather_temperature_tuple && weather_temperature_tuple->value->int16 != mTemperatureDegrees) {
    mTemperatureDegrees = weather_temperature_tuple->value->int16;
    weather_set_temperature(mTemperatureDegrees);
  }
  Tuple *weather_icon_tuple = dict_find(iter, WEATHER_ICON_KEY);
  if (weather_icon_tuple && weather_icon_tuple->value->uint8 != mTemperatureIcon) {
    mTemperatureIcon = weather_icon_tuple->value->uint8;
    weather_set_icon(mTemperatureIcon);
  }
  Tuple *weather_high_tuple = dict_find(iter, WEATHER_mTemperatureHigh_KEY);
  if (weather_high_tuple && weather_high_tuple->value->int16 != mTemperatureHigh) {
    mTemperatureHigh = weather_high_tuple->value->int16;
    setHighLow = true;
  }
  Tuple *weather_low_tuple = dict_find(iter, WEATHER_mTemperatureLow_KEY);
  if (weather_low_tuple && weather_low_tuple->value->int16 != mTemperatureLow) {
    mTemperatureLow = weather_low_tuple->value->int16;
    setHighLow = true;
  }
  if(setHighLow) {
    weather_set_highlow(mTemperatureHigh, mTemperatureLow);
  }
}

static void fetch_data(void) {

  Tuplet style_tuple = TupletInteger(STYLE_KEY, 0);
  Tuplet bluetoothvibe_tuple = TupletInteger(BLUETOOTHVIBE_KEY, 0);
  Tuplet hourlyvibe_tuple = TupletInteger(HOURLYVIBE_KEY, 0);
  Tuplet blink_tuple = TupletInteger(BLINK_KEY, 0);
  Tuplet dateformat_tuple = TupletInteger(DATEFORMAT_KEY, 0);
  Tuplet units_tuple = TupletInteger(WEATHER_UNITS, 0);
  Tuplet weather_temperature_tuple = TupletInteger(WEATHER_TEMPERATURE_KEY, 0);
  Tuplet weather_icon_tuple = TupletInteger(WEATHER_ICON_KEY, 0);
  Tuplet weather_high_tuple = TupletInteger(WEATHER_mTemperatureHigh_KEY, 0);
  Tuplet weather_low_tuple = TupletInteger(WEATHER_mTemperatureLow_KEY, 0);

  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  if (iter == NULL) {
    return;
  }

  dict_write_tuplet(iter, &style_tuple);
  dict_write_tuplet(iter, &bluetoothvibe_tuple);
  dict_write_tuplet(iter, &hourlyvibe_tuple);
  dict_write_tuplet(iter, &units_tuple);
  dict_write_tuplet(iter, &weather_temperature_tuple);
  dict_write_tuplet(iter, &weather_icon_tuple);
  dict_write_tuplet(iter, &weather_high_tuple);
  dict_write_tuplet(iter, &weather_low_tuple);
  dict_write_tuplet(iter, &blink_tuple);
  dict_write_tuplet(iter, &dateformat_tuple);
  dict_write_end(iter);

  app_message_outbox_send();
}

static void app_message_init(void) {
  app_message_register_inbox_received(in_received_handler);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  fetch_data();
}

void handle_init(void) {

  // WINDOW //
  mWindow = window_create();
  if (mWindow == NULL) {
      return;
  }
  window_stack_push(mWindow, true /* Animated */);
  mWindowLayer = window_get_root_layer(mWindow);
  window_set_background_color(mWindow, GColorWhite);
  
  // BACKGROUND //
  mBackgroundLayer = layer_create(layer_get_frame(mWindowLayer));
  layer_add_child(mWindowLayer, mBackgroundLayer);
  layer_set_update_proc(mBackgroundLayer, update_background_callback);

  // FONTS //
	ResHandle res_d = resource_get_handle(RESOURCE_ID_SMALL_26);
	ResHandle res_t = resource_get_handle(RESOURCE_ID_BIG_52);
	ResHandle res_temp = resource_get_handle(RESOURCE_ID_MEDIUM_34);
	ResHandle res_hl = resource_get_handle(RESOURCE_ID_SMALL_22);
  
	mDateFont = fonts_load_custom_font(res_d);
	mTimeFont = fonts_load_custom_font(res_t);
	mTemperatureFont = fonts_load_custom_font(res_temp);
	mHighLowFont = fonts_load_custom_font(res_hl);
  
  // TIME LAYER //
  
  mTimeLayer = layer_create(layer_get_frame(mWindowLayer));
  layer_add_child(mWindowLayer, mTimeLayer);
  
  /*
  mTimeLayer = text_layer_create(TIME_FRAME);  
	text_layer_set_background_color(mTimeLayer, GColorClear);
  text_layer_set_text_color(mTimeLayer, GColorBlack);
	text_layer_set_font(mTimeLayer, mTimeFont);
	text_layer_set_text_alignment(mTimeLayer, GTextAlignmentCenter);
	layer_add_child(mWindowLayer, text_layer_get_layer(mTimeLayer));
  */
  
  // TIME HOUR LAYER //
  mTimeHourLayer = text_layer_create(TIME_HOUR_FRAME);  
	text_layer_set_background_color(mTimeHourLayer, GColorClear);
  text_layer_set_text_color(mTimeHourLayer, GColorBlack);
	text_layer_set_font(mTimeHourLayer, mTimeFont);
	text_layer_set_text_alignment(mTimeHourLayer, GTextAlignmentRight);
	layer_add_child(mTimeLayer, text_layer_get_layer(mTimeHourLayer));
  
  // TIME SEPARATOR LAYER //
  mTimeSeparatorLayer = text_layer_create(TIME_SEP_FRAME);  
	text_layer_set_background_color(mTimeSeparatorLayer, GColorClear);
  text_layer_set_text_color(mTimeSeparatorLayer, GColorBlack);
	text_layer_set_font(mTimeSeparatorLayer, mTimeFont);
	text_layer_set_text_alignment(mTimeSeparatorLayer, GTextAlignmentCenter);
  text_layer_set_text(mTimeSeparatorLayer, ":");
	layer_add_child(mTimeLayer, text_layer_get_layer(mTimeSeparatorLayer));
  
  // TIME MINUTES LAYER //
  mTimeMinutesLayer = text_layer_create(TIME_MIN_FRAME);  
	text_layer_set_background_color(mTimeMinutesLayer, GColorClear);
  text_layer_set_text_color(mTimeMinutesLayer, GColorBlack);
	text_layer_set_font(mTimeMinutesLayer, mTimeFont);
	text_layer_set_text_alignment(mTimeMinutesLayer, GTextAlignmentLeft);
	layer_add_child(mTimeLayer, text_layer_get_layer(mTimeMinutesLayer));  
  
  
    
  // DATE LAYER //
  mDateLayer = text_layer_create(DATE_FRAME);  
	text_layer_set_background_color(mDateLayer, GColorClear);
  text_layer_set_text_color(mDateLayer, GColorBlack);
	text_layer_set_font(mDateLayer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));//mDateFont);
	text_layer_set_text_alignment(mDateLayer, GTextAlignmentCenter);
	layer_add_child(mWindowLayer, text_layer_get_layer(mDateLayer)); 
  	
  // WEATHER ICON //
  mWeatherIconLayer = bitmap_layer_create(WEATHER_ICON_FRAME);
  layer_add_child(mWindowLayer, bitmap_layer_get_layer(mWeatherIconLayer));
  
	// TEMPERATURE //
  mTemperatureLayer = text_layer_create(WEATHER_TEMP_FRAME);  
	text_layer_set_background_color(mTemperatureLayer, GColorClear);
  text_layer_set_text_color(mTemperatureLayer, GColorBlack);
	text_layer_set_font(mTemperatureLayer, mTemperatureFont); //fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD)); //
	text_layer_set_text_alignment(mTemperatureLayer, GTextAlignmentCenter);
	layer_add_child(mWindowLayer, text_layer_get_layer(mTemperatureLayer));

	// HIGHLOW //
  mHighLowLayer = text_layer_create(WEATHER_HL_FRAME);  
	text_layer_set_background_color(mHighLowLayer, GColorClear);
  text_layer_set_text_color(mHighLowLayer, GColorBlack);
	text_layer_set_font(mHighLowLayer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD)); //mHighLowFont);
	text_layer_set_text_alignment(mHighLowLayer, GTextAlignmentCenter);
	layer_add_child(mWindowLayer, text_layer_get_layer(mHighLowLayer));

	weather_set_loading();
  
  //mConfigStyle = 4;
  //setStyle();
  
  app_message_init();

  time_t now = time(NULL);
  struct tm *tick_time = localtime(&now);  
  //mInitialMinute = (tick_time->tm_min % FREQUENCY_MINUTES);
  
  handle_tick(tick_time, SECOND_UNIT + MINUTE_UNIT + HOUR_UNIT + DAY_UNIT);
  tick_timer_service_subscribe(SECOND_UNIT, handle_tick);
  
}



/*
void request_weather() {
	if(!located) {
		http_location_request();
		return;
	}
	// Build the HTTP request
	DictionaryIterator *body;
	HTTPResult result = http_out_get(SERVER_URL, WEATHER_HTTP_COOKIE, &body);
	if(result != HTTP_OK) {
		return;
	}
	dict_write_int32(body, WEATHER_KEY_LATITUDE, our_latitude);
	dict_write_int32(body, WEATHER_KEY_LONGITUDE, our_longitude);
	dict_write_cstring(body, WEATHER_KEY_UNIT_SYSTEM, UNIT_SYSTEM); //units
	// Send it.
	if(http_out_send() != HTTP_OK) {
		return;
	}
}
*/

void handle_deinit(void) {

  fonts_unload_custom_font(mTimeFont);
  fonts_unload_custom_font(mDateFont);
	fonts_unload_custom_font(mTemperatureFont);
	fonts_unload_custom_font(mHighLowFont);

  layer_remove_from_parent(bitmap_layer_get_layer(mWeatherIconLayer));
  bitmap_layer_destroy(mWeatherIconLayer);
  
  tick_timer_service_unsubscribe();
  
  window_destroy(mWindow);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}