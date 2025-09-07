// FILE: api/mousersupplier.h
#pragma once
#include "partsupplier.h"
#include "utils.h"
#include "partdata.h"
#include "config.h"

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
#include <QTextEdit>
#include <QPointer>

//extern QVBoxLayout* resultsLayout;
class MouserSupplier : public PartSupplier {
    friend class MainWindow;
    Q_OBJECT
public:
    explicit MouserSupplier(QNetworkAccessManager& mgr, QObject* parent = nullptr);
    ~MouserSupplier() override = default;
    QString name() const override { return "Mouser"; }
    QNetworkRequest                     searchRequest(const QString& keyword, int limit, int offset, QByteArray &outPayload) override;
    QList<QJsonObject>                  parseResults(const QByteArray &response) override;
    int                                 totalFromJson(const QByteArray &response);
    PartData                            toPartData(const QJsonObject& part) override;
private:
    void                                fetchImageIntoWidget(const QString& url, QLabel* image) override;
    QList<PriceBreak>                   parsePriceBreaks(const QJsonObject& part);
    std::tuple<QString,QString,QString> parseAvailabilityOnOrder(const QJsonObject& part);

    //QWidget*                            createPartCard(const QJsonObject &part) override; 
    //void fetchThumbnail(PartData& part, std::function<void(QPixmap)> callback)  override;
    //void appendResults(const QByteArray &json_data) override;
};