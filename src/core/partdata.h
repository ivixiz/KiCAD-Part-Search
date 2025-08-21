// FILE: core/partdata.h
#pragma once

#include "core/pricebreak.h"
#include "core/utils.h"
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariant>
#include <QPixmap>
#include <QFontMetrics>
#include <QtMath>


struct PartData {
    QString  partNumber;
    QString  manufacturer;
    QString  description;
    QString  datasheetUrl;
    QString  availability;
    float    price;
    int      stock;
    QPixmap  image;    
    QString  imageUrl; 
    QList<PriceBreak> priceBreaks;

    static PartData fromJson(const QJsonObject& obj);
};

Q_DECLARE_METATYPE(PartData)