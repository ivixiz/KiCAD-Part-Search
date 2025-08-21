#pragma once

#include "core/pricebreak.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QLabel>
#include <QFrame>
#include <QString>
#include <QList>

extern QLabel* debugOutputLabel;

void      logHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
QString   getStringField(const QJsonObject& obj, const QString& key);
QString   getField      (const QJsonObject& obj, const QString& key, const QString& def = "-");
std::tuple<QString,QString,QString> parseAvailabilityOnOrder(const QJsonObject& part);
QList<PriceBreak>                 parsePriceBreaks(const QJsonObject& part);
QLabel*   selectableLabel(const QString& html, bool rich = false);
QFrame*   vPricesSeparator(int w = 1, const QString& color = "#444");
int totalFromJson(const QByteArray &response);
