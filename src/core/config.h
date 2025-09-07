//FILE: core/config.h
#pragma once
#include <QString>
#include <vector>
#define SVAL(idx) settings[idx].val
#define SSTR(idx) settings[idx].str

enum SettingIndex {
    IMG_RESULT_SIZE_X,
    IMG_RESULT_SIZE_Y,
    WINDOW_SIZE_X,
    WINDOW_SIZE_Y,
    SUPPLIER_DEFAULT,
    REQUEST_LIMIT,
    VAT,
    LIMIT_RESULTS,
    SCROLL_STEP,
    CARD_MARGIN,
    DESCR_FIELD_SIZE,
    PRICE_FIELD_SIZE,
    ICON_SETTINGS_SIZE,
    ICON_COMBOBOX_SIZE,
    ICON_SETTINGS_PATH,
    ICON_MOUSER_PATH,
    ICON_DIGIKEY_PATH,
    CLIENTID_DIGIKEY,
    CLIENT_SECRET,
    TOKEN_URL,
    SEARCH_URL,
    API_KEY_MOUSER,
    SETTING_COUNT // всегда последний, чтобы знать размер массива
};

typedef enum { DIGIKEY = 0, MOUSER = 1 } SupplierEnum;

struct SupplierType { int id; QString name; QString iconPath; };
extern const std::vector<SupplierType> suppliers;
struct Settings { int id; int val; QString name; QString str; };
extern Settings settings[SETTING_COUNT];
