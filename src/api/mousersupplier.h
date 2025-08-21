// FILE: api/mousersupplier.h
#pragma once
#include "partsupplier.h"
#include "utils.h"
#include "partdata.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QLabel>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QPixmap>
#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>

#define IMG_RESULT_SIZE_X 120
#define IMG_RESULT_SIZE_Y 120

//extern QVBoxLayout* resultsLayout;
extern QNetworkAccessManager* globalNetMgr;

class MouserSupplier : public PartSupplier {
public:
    QString name() const override { return "Mouser"; }
    QNetworkRequest                     searchRequest(const QString& keyword, int offset, QByteArray &outPayload) override;
    QList<QJsonObject>                  parseResults(const QByteArray &response) override;
    QWidget*                            createPartCard(const QJsonObject &part) override;
    int                                 totalFromJson(const QByteArray &response);
private:
    void                                fetchImageIntoWidget(const QString& url, QLabel* image) override;
    QList<PriceBreak>                   parsePriceBreaks(const QJsonObject& part);
    std::tuple<QString,QString,QString> parseAvailabilityOnOrder(const QJsonObject& part);
    
    //void fetchThumbnail(PartData& part, std::function<void(QPixmap)> callback)  override;
    //void appendResults(const QByteArray &json_data) override;
};