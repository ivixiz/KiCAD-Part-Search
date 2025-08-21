// FILE: api/partsupplier.h

#pragma once
//#include "partdata.h"

#include <QString>
#include <QJsonObject>
#include <QWidget>
#include <QNetworkRequest> 
#include <QLabel>


class PartSupplier {
public:
    virtual ~PartSupplier() = default;

    virtual QString name() const = 0;
    virtual QNetworkRequest     searchRequest(const QString&, int, QByteArray&) = 0;
    virtual QList<QJsonObject>  parseResults(const QByteArray&) = 0;
    virtual QWidget*            createPartCard(const QJsonObject&) = 0;
    virtual void                fetchImageIntoWidget(const QString&, QLabel*) = 0;
    virtual int                 totalFromJson(const QByteArray&) = 0;
    //virtual void fetchThumbnail(PartData& part, std::function<void(QPixmap)> callback) = 0;
    //virtual void appendResults(const QByteArray&) = 0;
};