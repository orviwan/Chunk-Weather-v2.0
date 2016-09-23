#include <pebble.h>

const char* get_gcolor_text(GColor m_color);
void replace_gbitmap_color(GColor color_to_replace, GColor replace_with_color, GBitmap *im, BitmapLayer *bml);
void swap_gbitmap_color(GColor color_to_replace, GColor replace_with_color, GBitmap *im, BitmapLayer *bml);
void spit_gbitmap_color_palette(GBitmap *im);
