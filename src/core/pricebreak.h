#pragma once
#include <QString>

struct PriceBreak {
    int qty;
    float price;
    QString curr;

    PriceBreak() = default;
    PriceBreak(unsigned int q, float p, const QString& c)
        : qty(q), price(p), curr(c) {}
};