
//FILE core/config.cpp
#include "config.h"


Settings settings[SETTING_COUNT] = { //see also enum in config.h, must be same sequence
    {VAT,                       23,  "VAT",                      ""},
    {API_KEY_MOUSER,            0,   "MOUSER_API_KEY",           ""},
    {DIGIKEY_CLIENTID,          0,   "DIGIKEY_CLIENTID",         ""},
    {DIGIKEY_CLSECRET,          0,   "DIGIKEY_CLSECRET",         ""},
    {DIGIKEY_TOKEN_URL,         0,   "DIGIKEY_TOKEN_URL",        "https://api.digikey.com/v1/oauth2/token"},
    {DIGIKEY_SEARCH_URL,        0,   "DIGIKEY_SEARCH_URL",       "https://api.digikey.com/products/v4/search/keyword"},
    {EXPORT_FLAGS,              206, "EXPORT_FLAGS",             ""}, 
    {SUPPLIER,                  0,   "SUPPLIER_DEFAULT",         ""},
    {REQUEST_LIMIT,             30,  "REQUEST_LIMIT",            ""},   
    {LIMIT_RESULTS,             10,  "LIMIT_RESULTS",            ""},   
    {IMG_RESULT_SIZE_X,         120, "IMG_RESULT_SIZE_X",        ""},                          
    {IMG_RESULT_SIZE_Y,         120, "IMG_RESULT_SIZE_Y",        ""},                       
    {MAINWINDOW_SIZE_X,         660, "MAINWINDOW_SIZE_X",        ""},         
    {MAINWINDOW_SIZE_Y,         530, "MAINWINDOW_SIZE_Y",        ""},       
    {SCROLL_STEP,               10,  "SCROLL_STEP",              ""},        
    {CARD_MARGIN,               32,  "CARD_MARGIN",              ""},        
    {DESCR_FIELD_SIZE,          40,  "DESCR_FIELD_SIZE",         ""},   
    {PRICE_FIELD_SIZE,          80,  "PRICE_FIELD_SIZE",         ""},   
    {ICON_SETTINGS_SIZE,        20,  "ICON_SETTINGS_SIZE",       ""}, 
    {ICON_COMBOBOX_SIZE,        16,  "ICON_COMBOBOX_SIZE",       ""}, 
    {ICON_SETTINGS_PATH,        0,   "ICON_SETTINGS_PATH",       ":/icons/settings.svg"},
    {ICON_MOUSER_PATH,          0,   "ICON_MOUSER_PATH",         ":/icons/16x16px-mouser.png"},
    {ICON_DIGIKEY_PATH,         0,   "ICON_DIGIKEY_PATH",        ":/icons/16x16px-digikey.png"},
    {ICON_PARTSEARCH_PATH,      0,   "ICON_PARTSEARCH_PATH",     ":/icons/512x512-partSearch.png"},
    {SETTINGS_TEXTBOX_SIZE_Y,   30,  "SETTINGS_TEXTBOX_SIZE_Y",  ""},
    {CONFIG_FILENAME,           16,  "CONFIG_FILENAME",          "config.cfg"}, 
};

const std::vector<SupplierType> suppliers = {
    {0, "Mouser",  SSTR(ICON_MOUSER_PATH)},
    {1, "DigiKey", SSTR(ICON_DIGIKEY_PATH)}
}; //add others here, see also void MainWindow::initSupplier

const ExportFields exportFields[EXPORT_FIELDS_SIZE] = {
    {PRTNM, "Part Number"  },
    {MFRNO, "Mfr. No"      },
    {MFR,   "Mfr."         },
    {DESCR, "Description"  },
    {AVAIL, "Availability" },
    {PRURL, "Product URL"  },
    {DSURL, "Datasheet URL"},
    {SUPPL, "Supplier"     },
    {QTY,   "Quantity"     },
    {PRICE, "Price"        }
};