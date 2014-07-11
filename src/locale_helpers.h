#ifndef LOCALE_H
#define LOCALE_H

typedef enum {ENGLISH, ITALIAN, GERMAN, FRENCH, RUSSIAN} language_t;
	
const char *locale_month_name(int month, language_t language);
const char *locale_day_name(int day, language_t language);
const char *locale_low(language_t language);
const char *locale_high(language_t language);

#endif // LOCALE_H
	
