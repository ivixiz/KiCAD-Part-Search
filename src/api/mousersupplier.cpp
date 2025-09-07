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
    if (url.isEmpty() || !image)
        return;
    QString imgUrl = url;
    QNetworkRequest imgReq(imgUrl);
    imgReq.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64)");
    imgReq.setRawHeader("Accept", "image/avif,image/webp,image/apng,image/*,/*;q=0.8");
    imgReq.setRawHeader("Accept-Language", "en-US,en;q=0.9");
    imgReq.setRawHeader("Referer", "https://www.mouser.com/");
    QNetworkReply* rep = m_netMgr.get(imgReq);
    QPointer<QLabel> imagePtr(image);
    QObject::connect(rep, &QNetworkReply::finished, [rep, imagePtr]() mutable {
        if (!rep) return;
        QByteArray data = rep->readAll();
        rep->deleteLater();
        if (!imagePtr) {
            return;
        }
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
    pd.prdUrl = getField(part, "ProductDetailUrl", QString());
    pd.dsUrl  = getField(part, "DataSheetUrl", pd.prdUrl);
    pd.breaks = parsePriceBreaks(part);
    return pd;
}


// QWidget* MouserSupplier::createPartCard(const QJsonObject &part) {
//     // Extract fields
//     QString imgUrl = part.value("ImagePath").toString();
//     QString msrno  = getField(part, "MouserPartNumber");
//     QString avail  = getField(part, "Availability", "Non-Stocked");
//     QString curr   = getField(part, "Currency", "_");
//     QString mfrno  = getField(part, "ManufacturerPartNumber");
//     QString mfr    = getField(part, "Manufacturer");
//     QString descr  = getField(part, "Description");
//     QString prdUrl = getField(part, "ProductDetailUrl", QString());
//     QString dsUrl  = getField(part, "DataSheetUrl", prdUrl);
//     auto [order, ordqnt, orddate] = parseAvailabilityOnOrder(part);
//     QList<PriceBreak> breaks = parsePriceBreaks(part);
//     // ................... Main shell of result card ....................
//     QWidget* card = new QWidget;
//     auto hCard = new QHBoxLayout(card);
//     hCard->setContentsMargins(2,2,2,2);
//     hCard->setSpacing(4);

//     // ........................ Image + Info  ...........................
//     QLabel* image = new QLabel;
//     image->setFixedSize(IMG_RESULT_SIZE_X, IMG_RESULT_SIZE_Y);
//     hCard->addWidget(image);

//     // ......................... Load image .............................
    
//     if (!imgUrl.isEmpty()) {
//         if (imgUrl.startsWith('/')) imgUrl.prepend("https://www.mouser.com");
//         fetchImageIntoWidget(imgUrl, image);
//     }

//     auto leftCol = new QVBoxLayout;
//     leftCol->addWidget(selectableLabel(QString("<b>Mouser No:</b> %1").arg(msrno), true));
//     leftCol->addWidget(selectableLabel(QString("<b>Mfr. No:</b> %1").arg(mfrno), true));
//     leftCol->addWidget(selectableLabel(QString("<b>Mfr.:</b> %1").arg(mfr), true));
//     leftCol->addWidget(selectableLabel(QString("<b>Description:</b><br>%1").arg(descr), true));

//     auto datasheetLabel = new QLabel(QString("<b>Datasheet:</b> <a href='%1'>Link</a>").arg(dsUrl));
//     datasheetLabel->setTextFormat(Qt::RichText);
//     datasheetLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
//     datasheetLabel->setOpenExternalLinks(true);
//     leftCol->addWidget(datasheetLabel);

//     leftCol->addStretch();
//     hCard->addLayout(leftCol, 1);

//     auto rightCol = new QVBoxLayout;
//     QString availText = QString("<b>Availability:</b> %1").arg(avail);
//     if (ordqnt != "0"){ availText += QString(" (%1 expected %2)").arg(ordqnt, orddate); }  
//     QLabel* availLabel = selectableLabel(availText, true);
//     availLabel->setStyleSheet(avail.contains("In Stock", Qt::CaseInsensitive)
//                               ? "color: #00FF14" : "color: red");
//     rightCol->addWidget(availLabel);

//     auto hdr = new QHBoxLayout;
//     hdr->addWidget(selectableLabel("<b>Quantity</b>", true));
//     hdr->addWidget(selectableLabel("<b>Unit Price</b>", true));
//     hdr->addWidget(selectableLabel(QString("<b>Total+VAT%1%</b>")
//             .arg((VAT - 1.0) * 100.0, 0, 'f', 0), true));
//     rightCol->addLayout(hdr);
//     auto pricesContainer = new QWidget;
//     auto pricesLayout = new QVBoxLayout(pricesContainer);
//     pricesLayout->setContentsMargins(0,0,0,0);
//     pricesLayout->setSpacing(4);

//     for (const auto &pb : breaks) {
//         float ext = pb.price * pb.qty * VAT;
//         auto row = new QHBoxLayout;
//         row->addWidget(selectableLabel(QString::number(pb.qty)));
//         row->addWidget(selectableLabel(QString(" %1 %2")
//                                             .arg(QString::number(pb.price, 'f', 2), pb.curr)));
//         row->addWidget(selectableLabel(QString(" %1 %2")
//                                             .arg(QString::number(ext, 'f', 2), pb.curr)));
//         pricesLayout->addLayout(row);
//     }

//     auto scrollPrices = new QScrollArea;
//     scrollPrices->setWidget(pricesContainer);
//     scrollPrices->setWidgetResizable(true);

//     scrollPrices->setFixedHeight(100);
//     scrollPrices->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

//     rightCol->addWidget(scrollPrices);
//     rightCol->addStretch();

//     // ......................... Bold vertical line .............................
//     QFrame* vline = new QFrame;
//     vline->setFrameShape(QFrame::VLine);
//     vline->setFrameShadow(QFrame::Sunken);
//     vline->setStyleSheet("background-color: #aaaaaa;");
//     hCard->addWidget(vline);

//     hCard->addLayout(rightCol, 1);
//     return card;
//}
// void MouserSupplier::appendResults(const QByteArray &json_data) {
//     QJsonParseError err;
//     QJsonDocument doc = QJsonDocument::fromJson(json_data, &err);
//     if (err.error != QJsonParseError::NoError) {
//         qWarning() << "JSON parse error:" << err.errorString();
//         return;
//     }
//     QJsonObject root   = doc.object();
//     QJsonObject sr     = root.value("SearchResults").toObject();
//     QJsonArray parts   = sr.value("Parts").toArray();
//     int cnt = 0;
//     for (const QJsonValue &v : parts) {
//         if (!v.isObject() || cnt++ >= 30) break;
//         QWidget* card = MouserSupplier::createPartCard(v.toObject());
//         resultsLayout->addWidget(card);
//         // разделитель
//         QFrame* sep = new QFrame;
//         sep->setFrameShape(QFrame::HLine);
//         sep->setFrameShadow(QFrame::Sunken);
//         sep->setStyleSheet("margin:8px 0;");
//         resultsLayout->addWidget(sep);
//     }
// } 
// void MouserSupplier::fetchThumbnail(PartData& part, std::function<void(QPixmap)> callback) {  //fetch images of items
//     if (part.imageUrl.isEmpty()) return;

//     QNetworkRequest req(part.imageUrl);
//     req.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64)");
//     req.setRawHeader("Accept", "image/avif,image/webp,image/apng,image/*,*/*;q=0.8");
//     req.setRawHeader("Accept-Language", "en-US,en;q=0.9");
//     req.setRawHeader("Referer", "https://www.mouser.com/");
    
//     QNetworkReply* reply = globalNetMgr->get(req);
//     QObject::connect(reply, &QNetworkReply::finished, [reply, callback]() {
//         QPixmap pix;
//         if (pix.loadFromData(reply->readAll())) {
//             callback(pix);
//         }
//         reply->deleteLater();
//     });
// }

