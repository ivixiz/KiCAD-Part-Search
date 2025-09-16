// FILE: api/digikeysupplier.cpp 
#include "digikeysupplier.h"

DigikeySupplier::DigikeySupplier(QNetworkAccessManager& mgr, QObject* parent)
    : PartSupplier(mgr, parent) 
{
    fetchTokenSync();
    
}// ############################################ FUNCTION END ################################################################
void DigikeySupplier::fetchTokenSync() {
    qDebug() << "DigikeySupplier Initializing...";
    QNetworkRequest req{ QUrl(SSTR(DIGIKEY_TOKEN_URL)) };
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QUrlQuery body;
    body.addQueryItem("grant_type", "client_credentials");
    body.addQueryItem("client_id", SSTR(DIGIKEY_CLIENTID));
    body.addQueryItem("client_secret", SSTR(DIGIKEY_CLSECRET));

    QEventLoop loop;
    QNetworkReply* rep = m_netMgr.post(req, body.toString(QUrl::FullyEncoded).toUtf8());
    QObject::connect(rep, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    QByteArray resp = rep->readAll();
    rep->deleteLater();

    QJsonObject obj = QJsonDocument::fromJson(resp).object();
    accessToken_ = obj.value("access_token").toString();
    int expiresIn = obj.value("expires_in").toInt();
    tokenExpiry_ = QDateTime::currentDateTimeUtc().addSecs(expiresIn - 60);
    qDebug() << "DigikeySupplier Initialized";
}// ############################################ FUNCTION END ################################################################
#ifdef V4
void DigikeySupplier::ensureToken() {
    if (accessToken_.isEmpty() || QDateTime::currentDateTimeUtc() >= tokenExpiry_) {
        fetchTokenSync();
    }
}// ############################################ FUNCTION END ################################################################
QNetworkRequest DigikeySupplier::searchRequest(const QString& keyword,
                                               int limit,
                                               int offset,
                                               QByteArray& outPayload)
{
    ensureToken();

    const int lim = qMax(1, limit);      // PAGE_SIZE > 0
    const int off = qMax(0, offset);         // offset >= 0, V4: 0-based

    QJsonObject root;
    root["keywords"] = keyword;
    root["offset"]   = off;
    root["limit"]    = lim;

    outPayload = QJsonDocument(root).toJson(QJsonDocument::Compact);

    QNetworkRequest req{ QUrl(SSTR(DIGIKEY_SEARCH_URL)) };     // https://api.digikey.com/products/v4/search/keyword
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("Accept", "application/json");
    req.setRawHeader("Authorization", "Bearer " + accessToken_.toUtf8());
    req.setRawHeader("X-DIGIKEY-Client-Id", SSTR(DIGIKEY_CLIENTID).toUtf8());
    req.setRawHeader("X-DIGIKEY-Locale-Site", "US");
    req.setRawHeader("X-DIGIKEY-Locale-Language", "en");
    req.setRawHeader("X-DIGIKEY-Locale-Currency", "USD");
    req.setRawHeader("X-DIGIKEY-Customer-Id", "0");
    return req;
}// ############################################ FUNCTION END ################################################################
QList<QJsonObject> DigikeySupplier::parseResults(const QByteArray& resp) {
    QList<QJsonObject> list;
    const auto doc = QJsonDocument::fromJson(resp);
    if (!doc.isObject()) return list;

    const auto obj = doc.object();
    const auto arr = obj.value("Products").toArray();
    for (const auto &v : arr) if (v.isObject()) list.append(v.toObject());
    if (list.size() > SVAL(REQUEST_LIMIT))
        list = list.mid(0, SVAL(REQUEST_LIMIT));
    return list;
}// ############################################ FUNCTION END ################################################################
#else
void DigikeySupplier::ensureToken() {
    if (accessToken_.isEmpty() || QDateTime::currentDateTimeUtc() >= tokenExpiry_) {
        fetchTokenSync();
    }
}
QNetworkRequest DigikeySupplier::searchRequest(const QString& keyword,
                                               int offset,
                                               QByteArray& outPayload)
{
    ensureToken();

    // Формируем GET-запрос с параметрами
    QUrl url(BASE_V3_URL);
    QUrlQuery query;
    query.addQueryItem("Keywords", keyword);
    query.addQueryItem("RecordStartPosition", QString::number(offset + 1)); 
    query.addQueryItem("RecordCount", QString::number(PAGE_SIZE));          
    url.setQuery(query);

    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("Accept", "application/json");
    req.setRawHeader("Authorization", "Bearer " + accessToken_.toUtf8());
    req.setRawHeader("X-DIGIKEY-Client-Id", CLIENTID_DIGIKEY);
    req.setRawHeader("X-DIGIKEY-Locale-Site", "US");
    req.setRawHeader("X-DIGIKEY-Locale-Language", "en");
    req.setRawHeader("X-DIGIKEY-Locale-Currency", "USD");
    req.setRawHeader("X-DIGIKEY-Customer-Id", "0");

    outPayload.clear();  // Для V3 GET запроса тело не используется
    return req;
}
QList<QJsonObject> DigikeySupplier::parseResults(const QByteArray& response) {
    QList<QJsonObject> list;
    QJsonDocument doc = QJsonDocument::fromJson(response);
    qInfo() << "V3 fromJson == " << doc;

    QJsonArray products = doc.object().value("Products").toArray();
    for (const QJsonValue &v : products) {
        if (v.isObject())
            list.append(v.toObject());
    }
    return list;
}
#endif 
int DigikeySupplier::totalFromJson(const QByteArray& resp) {
    qDebug() << "Calculating total results...";
    const auto doc = QJsonDocument::fromJson(resp);
    if (!doc.isObject()) return 0;
    const auto obj = doc.object();
    return obj.value("ProductsCount").toInt(0);
}// ############################################ FUNCTION END ################################################################
QList<PriceBreak>  DigikeySupplier::parsePriceBreaks(const QJsonObject& part){
    //qDebug() << "Parsing price breaks...";
    QList<PriceBreak> breaks;
    QJsonArray arr = part.value("PriceBreaks").toArray();
    QJsonArray variations = part.value("ProductVariations").toArray();
    if (!variations.isEmpty()) {
        QJsonObject var0 = variations.first().toObject();
        QJsonArray stdPricing = var0.value("StandardPricing").toArray();
        for (const QJsonValue &pv : stdPricing) {
            QJsonObject o = pv.toObject();
            PriceBreak pb;
            pb.qty  = o.value("BreakQuantity").toInt();
            pb.price = (float)o.value("UnitPrice").toDouble();
            pb.curr = "USD";
            breaks.append(pb);
        }
    }
    return breaks;
}// ############################################ FUNCTION END ################################################################
void DigikeySupplier::fetchImageIntoWidget(const QString& url, QLabel* image) {
    qDebug() << "Fetching images...";
    if (url.isEmpty() || !image) { qWarning() << "Empty url or image widget is null"; return; }
    QString imgUrl = url;
    if (imgUrl.startsWith('/')) imgUrl.prepend("https://www.digikey.com");

    QNetworkRequest imgReq(imgUrl);
    imgReq.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64)");
    imgReq.setRawHeader("Accept", "image/avif,image/webp,image/apng,image/*,*/*;q=0.8");
    imgReq.setRawHeader("Accept-Language", "en-US,en;q=0.9");
    imgReq.setRawHeader("Referer", "https://www.digikey.com/");
    QNetworkReply* rep = m_netMgr.get(imgReq);
    QPointer<QLabel> imagePtr(image);

    QObject::connect(rep, &QNetworkReply::finished, [rep, imagePtr]() mutable {
        if (!rep) {return; qDebug() << "Error at fetchImageIntoWidget: network reply is nullptr"; }
        QByteArray data = rep->readAll();
        rep->deleteLater();
        if (!imagePtr) {return; qDebug() << "Error at fetchImageIntoWidget: image pointer is nullptr";}
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
PartData DigikeySupplier::toPartData(const QJsonObject& part) {
    //qDebug() << "Parsing json to part Data...";
    PartData pd;
    pd.suppl  = "DigiKey";
    pd.descr  = part["Description"].toObject().value("ProductDescription").toString();
    pd.mfr    = part["Manufacturer"].toObject().value("Name").toString();
    pd.mfrno  = part.value("ManufacturerProductNumber").toString();
    pd.prdUrl = part.value("ProductUrl").toString();
    pd.dsUrl  = part.value("DatasheetUrl").toString();
    pd.imgUrl = part.value("PhotoUrl").toString();
    QJsonArray variations = part.value("ProductVariations").toArray();
    if (!variations.isEmpty()) {
        QJsonObject firstVar = variations.first().toObject();
        pd.prtnm = firstVar.value("DigiKeyProductNumber").toString();
    } else {
        pd.prtnm = QString();
    }
    pd.avail  = QString::number(part.value("QuantityAvailable").toInt());
    pd.breaks = parsePriceBreaks(part);
    return pd;
}// ############################################ FUNCTION END ################################################################