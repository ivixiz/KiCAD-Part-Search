//FILE: core/config.h
#pragma once
#include <QString>
#include <vector>
#define SVAL(idx) settings[idx].val
#define SSTR(idx) settings[idx].str
#define SNM(idx) settings[idx].name
#define CONSTRAIN_VISIBLE_SETTINGS 6

enum SettingIndex {
    VAT,
    SUPPLIER_DEFAULT,
    API_KEY_MOUSER,
    DIGIKEY_CLIENTID,
    DIGIKEY_CLSECRET,
    DIGIKEY_TOKEN_URL,
    DIGIKEY_SEARCH_URL,
    REQUEST_LIMIT,
    LIMIT_RESULTS,
    IMG_RESULT_SIZE_X,
    IMG_RESULT_SIZE_Y,
    MAINWINDOW_SIZE_X,
    MAINWINDOW_SIZE_Y,
    SCROLL_STEP,
    CARD_MARGIN,
    DESCR_FIELD_SIZE,
    PRICE_FIELD_SIZE,
    ICON_SETTINGS_SIZE,
    ICON_COMBOBOX_SIZE,
    ICON_SETTINGS_PATH,
    ICON_MOUSER_PATH,
    ICON_DIGIKEY_PATH,
    CONFIG_FILENAME,
    SETTING_COUNT // всегда последний, чтобы знать размер массива
};

typedef enum { DIGIKEY = 0, MOUSER = 1 } SupplierEnum;

struct SupplierType { int id; QString name; QString iconPath; };
extern const std::vector<SupplierType> suppliers;
struct Settings { int id; int val; QString name; QString str; };
extern Settings settings[SETTING_COUNT];
