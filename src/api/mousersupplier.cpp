// FILE: api/mousersupplier.cpp
#include "mousersupplier.h"

MouserSupplier::MouserSupplier(QNetworkAccessManager& mgr, QObject* parent)
    : PartSupplier(mgr, parent)
{}// ############################################ FUNCTION END ################################################################
QNetworkRequest MouserSupplier::searchRequest(const QString& keyword, int limit, int offset, QByteArray &outPayload) { //send request to mouser to get info about keyword
    QString request = QString(
        "{\"SearchByKeywordRequest\": {"
        "\"apiKey\": \"%1\", "
        "\"keyword\": \"%2\", "
        "\"records\": %3, "
        "\"startingRecord\": %4, "
        "\"IncludeSearchResultsImages\": true}}"
    ).arg(SSTR(API_KEY_MOUSER))
     .arg(keyword)
     .arg(limit)
     .arg(offset + 1);
    outPayload = request.toUtf8();
    QNetworkRequest req(QUrl("https://api.mouser.com/api/v1/search/keyword?apiKey=" + QString(SSTR(API_KEY_MOUSER))));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    return req;
}
QList<QJsonObject> MouserSupplier::parseResults(const QByteArray &response) { //get results from mouser.com and paste it into the list
    QList<QJsonObject> list;
    QJsonDocument doc = QJsonDocument::fromJson(response);
    QJsonArray parts = doc.object()["SearchResults"].toObject()["Parts"].toArray();
    for (const auto& val : parts)
        if (val.isObject())
            list.append(val.toObject());
    return list;
}// ############################################ FUNCTION END ################################################################
void MouserSupplier::fetchImageIntoWidget(const QString& url, QLabel* image){
    //qDebug() << "Fetching images...";
    if (url.isEmpty() || !image){ return; }
    QString imgUrl = url;
    QNetworkRequest imgReq(imgUrl);
    imgReq.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64)");
    imgReq.setRawHeader("Accept", "image/avif,image/webp,image/apng,image/*,/*;q=0.8");
    imgReq.setRawHeader("Accept-Language", "en-US,en;q=0.9");
    imgReq.setRawHeader("Referer", "https://www.mouser.com/");
    QNetworkReply* rep = m_netMgr.get(imgReq);
    QPointer<QLabel> imagePtr(image);
    QObject::connect(rep, &QNetworkReply::finished, [rep, imagePtr]() mutable {
        if (!rep){ return; }
        QByteArray data = rep->readAll();
        rep->deleteLater();
        if (!imagePtr) { return; }
        QPixmap pix;
        if (pix.loadFromData(data)) {
            imagePtr->setPixmap(
                pix.scaled(
                    SVAL(IMG_RESULT_SIZE_X),
                    SVAL(IMG_RESULT_SIZE_Y),
                    Qt::KeepAspectRatio,
                    Qt::SmoothTransformation
                )
            );
        }
    });  
}// ############################################ FUNCTION END ################################################################
QList<PriceBreak>  MouserSupplier::parsePriceBreaks(const QJsonObject& part){
    QList<PriceBreak> breaks;
    QJsonArray arr = part.value("PriceBreaks").toArray();
    for (const auto &v : arr) {
        if (!v.isObject()) continue;
        auto o = v.toObject();
        unsigned int qty = o.value("Quantity").toInt(0);
        QString priceStr = o.value("Price").toString();
        float price = QLocale::c()
            .toDouble(priceStr.replace(",", ".")
                               .remove(QRegularExpression("[^0-9\\.]")));
        QString curr = o.value("Currency").toString();
        breaks.append({qty, price, curr});
    }
    return breaks;
}// ############################################ FUNCTION END ################################################################
std::tuple<QString,QString,QString> MouserSupplier::parseAvailabilityOnOrder(const QJsonObject& part) {
    QString order = "0", ordqnt = "0", orddate = "-";
    QJsonArray arr = part.value("AvailabilityOnOrder").toArray();
    if (!arr.isEmpty()) {
        QJsonObject first = arr.first().toObject();
        if (first.contains("Quantity") && first.value("Quantity").isDouble())
            ordqnt = QString::number(first.value("Quantity").toInt());
        if (first.contains("Date") && first.value("Date").isString())
            orddate = first.value("Date").toString().split('T').first();
        order = ordqnt;
    }
    return {order, ordqnt, orddate};
}// ############################################ FUNCTION END ################################################################
int MouserSupplier::totalFromJson(const QByteArray &response) {
    auto doc = QJsonDocument::fromJson(response);
    auto sr  = doc.object()
                  .value("SearchResults")
                  .toObject();
    return sr.value("NumberOfResult").toInt();
}// ############################################ FUNCTION END ################################################################
PartData MouserSupplier::toPartData(const QJsonObject& part) {
    PartData pd;
    pd.suppl = "Mouser";
    pd.imgUrl = getField(part, "ImagePath");
    pd.prtnm  = getField(part, "MouserPartNumber");
    pd.avail  = getField(part, "Availability", "Non-Stocked");
    pd.curr   = getField(part, "Currency", "_");
    pd.mfrno  = getField(part, "ManufacturerPartNumber");
    pd.mfr    = getField(part, "Manufacturer");
    pd.descr  = getField(part, "Description");
    pd.prUrl = getField(part, "ProductDetailUrl", QString());
    pd.dsUrl  = getField(part, "DataSheetUrl", pd.prUrl);
    pd.breaks = parsePriceBreaks(part);
    return pd;
}
