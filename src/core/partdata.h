// FILE: core/partdata.h
#pragma once
#include <QString>
#include <QList>
#include "pricebreak.h"
struct PartData {
    QString suppl;
    QString imgUrl;
    QString prtnm;
    QString avail;
    QString curr;
    QString mfrno;
    QString mfr;
    QString descr;
    QString prdUrl;
    QString dsUrl;
    QList<PriceBreak> breaks;
};
Q_DECLARE_METATYPE(PartData)