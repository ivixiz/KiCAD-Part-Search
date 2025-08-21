// FILE: api/digikeysupplier.cpp 

#include "digikeysupplier.h"
#define V4

static const QString BASE_V3_URL = QStringLiteral("https://api.digikey.com/products/v3/search/keyword");

DigikeySupplier::DigikeySupplier() {
    fetchTokenSync();
}

void DigikeySupplier::fetchTokenSync() {
    QNetworkRequest req(QUrl(TOKEN_URL));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QUrlQuery body;
    body.addQueryItem("grant_type", "client_credentials");
    body.addQueryItem("client_id", CLIENTID_DIGIKEY);
    body.addQueryItem("client_secret", CLIENT_SECRET);

    QEventLoop loop;
    QNetworkReply* rep = globalNetMgr->post(req, body.toString(QUrl::FullyEncoded).toUtf8());
    QObject::connect(rep, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    QByteArray resp = rep->readAll();
    rep->deleteLater();

    QJsonObject obj = QJsonDocument::fromJson(resp).object();
    accessToken_ = obj.value("access_token").toString();
    int expiresIn = obj.value("expires_in").toInt();
    tokenExpiry_ = QDateTime::currentDateTimeUtc().addSecs(expiresIn - 60);
}

#ifdef V4
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

    // Гарантии корректных значений
    const int limit = qMax(1, REQUEST_SIZE);      // PAGE_SIZE > 0
    const int off   = qMax(0, offset);         // offset >= 0, V4: 0-based

    QJsonObject root;
    root["keywords"] = keyword;
    root["offset"]   = off;                    // V4: смещение
    root["limit"]    = limit;                  // V4: размер страницы

    outPayload = QJsonDocument(root).toJson(QJsonDocument::Compact);

    QNetworkRequest req(QUrl(SEARCH_URL));     // https://api.digikey.com/products/v4/search/keyword
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("Accept", "application/json");
    req.setRawHeader("Authorization", "Bearer " + accessToken_.toUtf8());
    req.setRawHeader("X-DIGIKEY-Client-Id", CLIENTID_DIGIKEY);
    req.setRawHeader("X-DIGIKEY-Locale-Site", "US");
    req.setRawHeader("X-DIGIKEY-Locale-Language", "en");
    req.setRawHeader("X-DIGIKEY-Locale-Currency", "USD");
    req.setRawHeader("X-DIGIKEY-Customer-Id", "0");
    return req;
}
QList<QJsonObject> DigikeySupplier::parseResults(const QByteArray& resp) {
    QList<QJsonObject> list;
    const auto doc = QJsonDocument::fromJson(resp);
    if (!doc.isObject()) return list;

    const auto obj = doc.object();
    const auto arr = obj.value("Products").toArray();
    for (const auto &v : arr) if (v.isObject()) list.append(v.toObject());
    if (list.size() > REQUEST_SIZE)
        list = list.mid(0, REQUEST_SIZE);
    return list;
}
#endif

#ifdef V3
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
    const auto doc = QJsonDocument::fromJson(resp);
    if (!doc.isObject()) return 0;
    const auto obj = doc.object();
    return obj.value("ProductsCount").toInt(0);
}
QList<PriceBreak>  DigikeySupplier::parsePriceBreaks(const QJsonObject& part){
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
    if (url.isEmpty())
        return;
    QString imgUrl = url;
    if (imgUrl.startsWith('/'))
        imgUrl.prepend("https://www.digikey.com");
    QNetworkRequest imgReq(imgUrl);
    imgReq.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64)");
    imgReq.setRawHeader("Accept", "image/avif,image/webp,image/apng,image/*,*/*;q=0.8");
    imgReq.setRawHeader("Accept-Language", "en-US,en;q=0.9");
    imgReq.setRawHeader("Referer", "https://www.digikey.com/");  
    QNetworkReply* rep = globalNetMgr->get(imgReq);
    QObject::connect(rep, &QNetworkReply::finished, [rep, image]() {
        QByteArray data = rep->readAll();
        QPixmap pix;
        if (pix.loadFromData(data)) {
            image->setPixmap(
                pix.scaled(
                    IMG_RESULT_SIZE_X,
                    IMG_RESULT_SIZE_Y,
                    Qt::KeepAspectRatio,
                    Qt::SmoothTransformation
                )
            );
        }
        rep->deleteLater();
    });
}
QWidget* DigikeySupplier::createPartCard(const QJsonObject& part) {
    QString descr   = part["Description"].toObject().value("ProductDescription").toString();
    QString mfr     = part["Manufacturer"].toObject().value("Name").toString();
    QString mfrno   = part.value("ManufacturerProductNumber").toString();
    QString prdUrl  = part.value("ProductUrl").toString();
    QString dsUrl   = part.value("DatasheetUrl").toString();
    QString imgUrl  = part.value("PhotoUrl").toString();
    QString dkpn    = part.value("DigiKeyProductNumber").toString();
    QString avail   = QString::number(part.value("QuantityAvailable").toInt());
    QList<PriceBreak> breaks = parsePriceBreaks(part);
    // ................... Main shell of result card ....................
    QWidget* card = new QWidget;
    auto hCard = new QHBoxLayout(card);
    hCard->setContentsMargins(2,2,2,2);
    hCard->setSpacing(4);

    // ........................ Image + Info  ...........................
    QLabel* image = new QLabel;
    image->setFixedSize(IMG_RESULT_SIZE_X, IMG_RESULT_SIZE_Y);
    hCard->addWidget(image);

    // ......................... Load image .............................
    
    if (!imgUrl.isEmpty()) {
        if (imgUrl.startsWith('/')) imgUrl.prepend("https://www.digikey.com");
        fetchImageIntoWidget(imgUrl, image);
    }

    auto leftCol = new QVBoxLayout;
    leftCol->addWidget(selectableLabel(QString("<b>Digikey No:</b> %1").arg(dkpn), true));
    leftCol->addWidget(selectableLabel(QString("<b>Mfr. No:</b> %1").arg(mfrno), true));
    leftCol->addWidget(selectableLabel(QString("<b>Mfr.:</b> %1").arg(mfr), true));
    leftCol->addWidget(selectableLabel(QString("<b>Description:</b><br>%1").arg(descr), true));

    auto datasheetLabel = new QLabel(QString("<b>Datasheet:</b> <a href='%1'>Link</a>").arg(dsUrl));
    datasheetLabel->setTextFormat(Qt::RichText);
    datasheetLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    datasheetLabel->setOpenExternalLinks(true);
    leftCol->addWidget(datasheetLabel);
    leftCol->addStretch();
    hCard->addLayout(leftCol, 1);
    // ...................... Prices, Stock ................................
    auto rightCol = new QVBoxLayout;
    QString availText = QString("<b>Availability:</b> %1").arg(avail);
    QLabel* availLabel = selectableLabel(availText, true);
    if (avail != "0"){ availLabel->setStyleSheet("color: #00FF14");}
    else{            availLabel->setStyleSheet("color: red");}
    rightCol->addWidget(availLabel);

    // .................... Prices header, names ...........................
    auto hdr = new QHBoxLayout;
    hdr->addWidget(selectableLabel("<b>Quantity</b>", true));
    hdr->addWidget(selectableLabel("<b>Unit Price</b>", true));
    hdr->addWidget(selectableLabel(QString("<b>Total+VAT%1%</b>")
            .arg((VAT - 1.0) * 100.0, 0, 'f', 0), true));
    rightCol->addLayout(hdr);

    // ................... Rows of Prices(quantity) .........................
    auto pricesContainer = new QWidget;
    auto pricesLayout = new QVBoxLayout(pricesContainer);
    pricesLayout->setContentsMargins(0,0,0,0);
    pricesLayout->setSpacing(4);

    for (const auto &pb : breaks) {
        float ext = pb.price * pb.qty * VAT;
        auto row = new QHBoxLayout;
        row->addWidget(selectableLabel(QString::number(pb.qty)));
        row->addWidget(selectableLabel(QString(" %1 %2")
                                            .arg(QString::number(pb.price, 'f', 2), pb.curr)));
        row->addWidget(selectableLabel(QString(" %1 %2")
                                            .arg(QString::number(ext, 'f', 2), pb.curr)));
        pricesLayout->addLayout(row);
    }

    auto scrollPrices = new QScrollArea;
    scrollPrices->setWidget(pricesContainer);
    scrollPrices->setWidgetResizable(true);

    scrollPrices->setFixedHeight(100);
    scrollPrices->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    rightCol->addWidget(scrollPrices);
    rightCol->addStretch();

    // ......................... Bold vertical line .............................
    QFrame* vline = new QFrame;
    vline->setFrameShape(QFrame::VLine);
    vline->setFrameShadow(QFrame::Sunken);
    vline->setStyleSheet("background-color: #aaaaaa;");
    hCard->addWidget(vline);

    hCard->addLayout(rightCol, 1);
    return card;
}
