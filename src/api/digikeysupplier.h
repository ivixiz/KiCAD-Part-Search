// FILE: api/digikeysupplier.h
#pragma once
#include "partsupplier.h"
#include "utils.h"
#include "partdata.h"

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



#define IMG_RESULT_SIZE_X 120
#define IMG_RESULT_SIZE_Y 120

extern QNetworkAccessManager* globalNetMgr;
extern float VAT;
extern int REQUEST_SIZE;

#define CLIENTID_DIGIKEY "AAXLt5AGRxvXX4VzJa51MFzEGbttGvHh2LVPDzn5KZlN7GVi"
#define CLIENT_SECRET    "rE05nGDWBqtY8qw20TJMfUCDB4I7VADGTwg4PGmPJJEtiT2jaDDT0e7ACOAQMoHh"
#define TOKEN_URL        "https://api.digikey.com/v1/oauth2/token"
#define SEARCH_URL       "https://api.digikey.com/products/v4/search/keyword"

class DigikeySupplier : public PartSupplier {
public:
    DigikeySupplier();
    ~DigikeySupplier() override = default;

    QString name() const override { return "DigiKey"; }
    QNetworkRequest     searchRequest(const QString& keyword, int offset, QByteArray& outPayload) override;
    QList<QJsonObject>  parseResults(const QByteArray& response) override;
    QWidget*            createPartCard(const QJsonObject& part) override;
    void                fetchImageIntoWidget(const QString& url, QLabel* image) override;
    int                 totalFromJson(const QByteArray& resp);

private:
    QString accessToken_;
    QDateTime tokenExpiry_;

    void                ensureToken();  
    void                fetchTokenSync();               
    QList<PriceBreak>   parsePriceBreaks(const QJsonObject& part);

};