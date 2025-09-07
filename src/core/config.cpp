
//FILE core/config.cpp
#include "config.h"

Settings settings[SETTING_COUNT] = {
    {IMG_RESULT_SIZE_X,   120, "IMG_RESULT_SIZE_X", ""},                          
    {IMG_RESULT_SIZE_Y,   120, "IMG_RESULT_SIZE_Y", ""},                       
    {WINDOW_SIZE_X,       660, "WINDOW_SIZE_X", ""},         
    {WINDOW_SIZE_Y,       530, "WINDOW_SIZE_Y", ""},  
    {SUPPLIER_DEFAULT,    1,   "SUPPLIER_DEFAULT", ""},   
    {REQUEST_LIMIT,       30,  "REQUEST_LIMIT", ""},   
    {VAT,                 23,  "VAT", ""},//23%
    {LIMIT_RESULTS,       10,  "LIMIT_RESULTS", ""},        
    {SCROLL_STEP,         10,  "SCROLL_STEP", ""},        
    {CARD_MARGIN,         32,  "CARD_MARGIN", ""},        
    {DESCR_FIELD_SIZE,    40,  "DESCR_FIELD_SIZE", ""},   
    {PRICE_FIELD_SIZE,    80,  "PRICE_FIELD_SIZE", ""},   
    {ICON_SETTINGS_SIZE,  20,  "ICON_SETTINGS_SIZE", ""}, 
    {ICON_COMBOBOX_SIZE,  16,  "ICON_COMBOBOX_SIZE", ""}, 
    {ICON_SETTINGS_PATH,  0,   "ICON_SETTINGS_PATH", ":/icons/settings.svg"},
    {ICON_MOUSER_PATH,    0,   "ICON_MOUSER_PATH",   ":/icons/16x16px-mouser.png"},
    {ICON_DIGIKEY_PATH,   0,   "ICON_DIGIKEY_PATH",  ":/icons/16x16px-digikey.png"},
    {CLIENTID_DIGIKEY,    0,   "CLIENTID_DIGIKEY",   "AAXLt5AGRxvXX4VzJa51MFzEGbttGvHh2LVPDzn5KZlN7GVi"},
    {CLIENT_SECRET,       0,   "CLIENT_SECRET",      "rE05nGDWBqtY8qw20TJMfUCDB4I7VADGTwg4PGmPJJEtiT2jaDDT0e7ACOAQMoHh"},
    {TOKEN_URL,           0,   "TOKEN_URL",          "https://api.digikey.com/v1/oauth2/token"},
    {SEARCH_URL,          0,   "SEARCH_URL",         "https://api.digikey.com/products/v4/search/keyword"},
    {API_KEY_MOUSER,      0,   "API_KEY_MOUSER",     "f4c8cbf5-d08c-401c-974e-42a5b8535d52"}
};


const std::vector<SupplierType> suppliers = {
    {0, "DigiKey", SSTR(ICON_DIGIKEY_PATH)},
    {1, "Mouser",  SSTR(ICON_MOUSER_PATH)}
}; //add others here, see also void MainWindow::initSupplier


