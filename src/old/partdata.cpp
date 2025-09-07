// core/partdata.cpp
#include "partdata.h"

PartData PartData::fromJson(const QJsonObject& obj) {
    PartData p;
    p.partNumber   = obj["MouserPartNumber"].toString();
    p.manufacturer = obj["Manufacturer"].toString();
    p.description  = obj["Description"].toString();
    p.datasheetUrl = obj["DataSheetUrl"].toString();
    p.availability = obj["Availability"].toString();
    QJsonArray br  = obj.value("PriceBreaks").toArray();
    p.price = br.isEmpty() ? 0.0f
            : br.first().toObject()["Price"].toString().remove('$').toFloat();
    p.stock = obj["Availability"].toString().split(' ').first().toInt();
    p.priceBreaks = parsePriceBreaks(obj);
    p.image     = QPixmap(); 
    p.imageUrl  = obj["ImagePath"].toString();
    return p;
}
