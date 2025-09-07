// FILE: api/partsupplier.h
#pragma once
#include <QString>
#include <QJsonObject>
#include <QWidget>
#include <QLabel>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include "core/partdata.h"

class PartSupplier : public QObject {
    Q_OBJECT
public:
    explicit PartSupplier(QNetworkAccessManager& mgr, QObject* parent = nullptr)
        : QObject(parent), m_netMgr(mgr) {}

    virtual ~PartSupplier() = default;

    virtual QString name() const = 0;
    virtual QNetworkRequest searchRequest(const QString&, int, int, QByteArray&) = 0;
    virtual QList<QJsonObject> parseResults(const QByteArray&) = 0;
    virtual void fetchImageIntoWidget(const QString&, QLabel*) = 0;
    virtual int totalFromJson(const QByteArray&) = 0;
    virtual PartData toPartData(const QJsonObject& part) = 0;

protected:
    QNetworkAccessManager& m_netMgr;
};
