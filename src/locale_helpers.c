#include "locale_helpers.h"
	
const char *locale_month_name(int month, language_t language) {
    switch (language) {
        case ITALIAN: 
            switch(month) {
                case 0: return "GEN";
                case 1: return "FEB";
                case 2: return "MAR";
                case 3: return "APR";
                case 4: return "MAG";
                case 5: return "GUI";
                case 6: return "LUG";
                case 7: return "AGO";
                case 8: return "SETT";
                case 9: return "OTT";
                case 10: return "NOV";
                case 11: return "DIC";
                default: return "";
            };  
        ;
        case GERMAN: 
            switch(month) {
                case 0: return "JÄN";
                case 1: return "FEB";
                case 2: return "MÄRZ";
                case 3: return "APR";
                case 4: return "MAI";
                case 5: return "JUNI";
                case 6: return "JULI";
                case 7: return "AUG";
                case 8: return "SEPT";
                case 9: return "OKT";
                case 10: return "NOV";
                case 11: return "DEZ";
                default: return "";
            };  
        ;        
        case FRENCH: 
            switch(month) {
                case 0: return "JANV";
                case 1: return "FÉVR";
                case 2: return "MARS";
                case 3: return "AVRIL";
                case 4: return "MAI";
                case 5: return "JUIN";
                case 6: return "JUIL";
                case 7: return "AOÛT";
                case 8: return "SEPT";
                case 9: return "OCT";
                case 10: return "NOV";
                case 11: return "DÉC";
                default: return "";
            };  
        ;
        default:  
            switch(month) {
                case 0: return "JAN";
                case 1: return "FEB";
                case 2: return "MAR";
                case 3: return "APR";
                case 4: return "MAY";
                case 5: return "JUN";
                case 6: return "JUL";
                case 7: return "AUG";
                case 8: return "SEP";
                case 9: return "OCT";
                case 10: return "NOV";
                case 11: return "DEC";
                default: return "";
            };        
        ;
    };
}


const char *locale_day_name(int day, language_t language) {
    switch (language) {
        case ITALIAN: 
            switch(day) {
                case 0: return "DOM";
                case 1: return "LUN";
                case 2: return "MAR";
                case 3: return "MER";
                case 4: return "GIO";
                case 5: return "VEN";
                case 6: return "SAB";
                default: return "";
            };  
        ;
        case GERMAN: 
            switch(day) {
                case 0: return "SO";
                case 1: return "MO";
                case 2: return "DI";
                case 3: return "MI";
                case 4: return "DO";
                case 5: return "FR";
                case 6: return "SA";
                default: return "";
            };  
        ;
        case FRENCH: 
            switch(day) {
                case 0: return "DOM";
                case 1: return "LUN";
                case 2: return "MAR";
                case 3: return "MIÉR";
                case 4: return "JUE";
                case 5: return "VIÉR";
                case 6: return "SÁB";
                default: return "";
            };  
        ;         
        default: 
            switch(day) {
                case 0: return "SUN";
                case 1: return "MON";
                case 2: return "TUE";
                case 3: return "WED";
                case 4: return "THU";
                case 5: return "FRI";
                case 6: return "SAT";
                default: return "";
            };  
        ;
    };
}

const char *locale_low(language_t language) {
    switch (language) {
        case ITALIAN: 
        case GERMAN:
        case FRENCH:
            return "MIN"; 
        ;
        default: return "LOW";
    };
}
const char *locale_high(language_t language) {
    switch (language) {
        case ITALIAN: 
        case GERMAN:
        case FRENCH:
            return "MAX"; 
        ;
        default: 
            return "HIGH";
    };
}