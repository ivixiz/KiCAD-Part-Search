// FILE: api/digikeysupplier.h
#pragma once
#include "partsupplier.h"
#include "utils.h"
#include "partdata.h"
#include "config.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrlQuery>
#include <QEventLoop>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QHBoxLayout>
#include <QList>
#include <cfloat>
#include <QTextEdit>
#include <QPointer>

#define V4

static const QString BASE_V3_URL = QStringLiteral("https://api.digikey.com/products/v3/search/keyword");


class DigikeySupplier : public PartSupplier {
    friend class MainWindow;
    Q_OBJECT
public:
    explicit DigikeySupplier(QNetworkAccessManager& mgr, QObject* parent = nullptr);
    ~DigikeySupplier() override = default;

    QString name() const override { return "DigiKey"; }
    QNetworkRequest     searchRequest(const QString& keyword, int limit, int offset, QByteArray& outPayload) override;
    QList<QJsonObject>  parseResults(const QByteArray& response) override;
    void                fetchImageIntoWidget(const QString& url, QLabel* image) override;
    int                 totalFromJson(const QByteArray& resp);
    PartData            toPartData(const QJsonObject& part) override;

private:
    QString accessToken_;
    QDateTime tokenExpiry_;
    void                ensureToken();  
    void                fetchTokenSync();               
    QList<PriceBreak>   parsePriceBreaks(const QJsonObject& part);
};