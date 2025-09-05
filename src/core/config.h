//FILE: core/config.h
#pragma once

inline const int IMG_RESULT_SIZE_X  = 120;
inline const int IMG_RESULT_SIZE_Y  = 120;
inline const int WINDOW_SIZE_X      = 800;
inline const int WINDOW_SIZE_Y      = 600;
inline const int REQUEST_SIZE       = 30; //min 30 for Mouser, 10 faster for DigiKey
inline const float VAT              = 0.23f;
inline const int LIMIT_RESULTS      = 10; //actually will be size of response + LIMIT_RESULTS
inline const int SCROLL_STEP        = 10;
inline const int CARD_MARGIN        = 32;
inline const int DESCR_FIELD_SIZE   = 40;
inline const int PRICE_FIELD_SIZE   = 80;
inline const int ICON_SETTINGS_SIZE = 20;
inline const int ICON_COMBOBOX_SIZE = 16;

#define ICON_SETTINGS_PATH ":/icons/settings.svg"
#define ICON_MOUSER_PATH   ":/icons/16x16px-mouser.png"
#define ICON_DIGIKEY_PATH  ":/icons/16x16px-digikey.png"