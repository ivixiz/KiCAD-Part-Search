//FILE: core/config.h
#pragma once
#include <QString>
#include <vector>
#define SVAL(idx) settings[idx].val
#define SSTR(idx) settings[idx].str
#define SNM(idx)  settings[idx].name
#define EID(idx)  exportFields[idx].id
#define ENM(idx)  exportFields[idx].name
#define EXPORT_FIELDS_SIZE         10

#define CONSTRAIN_VISIBLE_SETTINGS 7
enum SettingIndex {
    VAT,
    API_KEY_MOUSER,
    DIGIKEY_CLIENTID,
    DIGIKEY_CLSECRET,
    DIGIKEY_TOKEN_URL,
    DIGIKEY_SEARCH_URL,
    EXPORT_FLAGS,
    SUPPLIER,
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
    ICON_PARTSEARCH_PATH,
    SETTINGS_TEXTBOX_SIZE_Y,
    CONFIG_FILENAME,
    //========================================
    SETTING_COUNT //always last = size of enum
};

typedef enum {
    MOUSER,
    DIGIKEY
} SupplierEnum;

typedef enum {
    PRTNM,
    MFRNO,
    MFR,
    DESCR,
    AVAIL,
    PRURL,
    DSURL,
    SUPPL,
    QTY,
    PRICE
} ExportEnum;

struct ExportFields { int id; QString name; };
extern const ExportFields exportFields[EXPORT_FIELDS_SIZE];
struct SupplierType { int id; QString name; QString iconPath; };
extern const std::vector<SupplierType> suppliers;
struct Settings { int id; int val; QString name; QString str; };
extern Settings settings[SETTING_COUNT];
