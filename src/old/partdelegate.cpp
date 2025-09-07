// ui/partdelegate.cpp
#include "partdelegate.h"

void PartDelegate::paint(QPainter* painter,
                         const QStyleOptionViewItem& opt,
                         const QModelIndex& idx) const
{
    PartData part = idx.data(Qt::UserRole + 1).value<PartData>();
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    // background
    painter->fillRect(opt.rect,
        (opt.state & QStyle::State_Selected) ? QColor("#58c2e2")
                                              : QColor("#1a1a1a"));
    QRectF bg = opt.rect.adjusted(4,4,-4,-4);
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor("#3f3f3f"));
    painter->drawRoundedRect(bg, 6, 6);

    // thumbnail
    QRect thumbR(bg.topLeft().toPoint() + QPoint(8,8), QSize(80,80));
    if (!part.image.isNull())
        painter->drawPixmap(thumbR, part.image);

    // columns geometry
    int x  = thumbR.right() + 12;
    int ry = bg.top() + 10;
    int rx = bg.right() - 180;  // price column left edge

    // --- Part Number + Manufacturer ---
    painter->setPen(Qt::white);
    QFont bold = painter->font();
    bold.setBold(true);
    painter->setFont(bold);
    painter->drawText(QRect(x, ry, rx - x - 10, 20),
                      Qt::AlignLeft|Qt::AlignVCenter,
                      part.partNumber + "  —  " + part.manufacturer);

    // --- Description ---
    ry += 22;
    painter->setFont(opt.font);
    int descHeight = 36;
    int descWidth  = rx - x - 10;   // <<< limit to before price column
    painter->drawText(QRect(x, ry, descWidth, descHeight),
                      Qt::TextWordWrap,
                      part.description);

    // --- Datasheet link (underlined) ---
    QFont link = opt.font; link.setUnderline(true);
    painter->setFont(link);
    painter->setPen(QColor("#5d5dff"));
    painter->drawText(QPoint(x, bg.bottom() - 12), "Datasheet");

    // --- Price column ---
    ry = bg.top() + 10;
    QFont small = opt.font; small.setPointSizeF(opt.font.pointSizeF() - 1);
    painter->setFont(small);

    // Availability
    painter->setPen(part.availability.contains("In Stock", Qt::CaseInsensitive)
                   ? QColor("#00FF14") : Qt::red);
    painter->drawText(QRect(rx, ry, 170, 20),
                      Qt::AlignLeft|Qt::AlignVCenter,
                      "Availability: " + part.availability);
    ry += 22;

    // Header
    painter->setPen(Qt::white);
    bold.setPointSizeF(small.pointSizeF());
    bold.setBold(true);
    painter->setFont(bold);
    painter->drawText(QRect(rx,    ry, 35, 16), Qt::AlignLeft, "Qty");
    painter->drawText(QRect(rx+60, ry, 45, 16), Qt::AlignLeft, "Unit");
    painter->drawText(QRect(rx+120,ry, 45, 16), Qt::AlignLeft, "Total");
    ry += 18;

    // Rows
    painter->setFont(small);
    for (auto &pb : part.priceBreaks) {
        float total = pb.price * pb.qty * VAT;
        painter->drawText(QRect(rx,    ry, 35, 16), Qt::AlignLeft,
                          QString::number(pb.qty));
        painter->drawText(QRect(rx+60, ry, 45, 16), Qt::AlignLeft,
                          QString("%1 %2")
                            .arg(QString::number(pb.price, 'f', 2), pb.curr));
        painter->drawText(QRect(rx+120,ry, 45, 16), Qt::AlignLeft,
                          QString("%1 %2")
                            .arg(QString::number(total, 'f', 2), pb.curr));
        ry += 18;
        if (ry > bg.bottom() - 20) break;
    }

    painter->restore();
}

QSize PartDelegate::sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const {
    return QSize(0, 100);
}

/*


    int rx = bg.right() - 180;
    int ry = bg.top() + 10;
    int rw = 150;

    QFont small = opt.font;
    QFontMetrics fm(small);
    int qtyWidth   = fm.horizontalAdvance("Qty") + 10;
    int priceWidth = 0;
    int totalWidth = 0;

    // Найдём максимальную ширину для Price и Total
    for (const auto& pb : part.priceBreaks) {
        QString unitStr  = QString::number(pb.price, 'f', 2) + " " + pb.curr;
        QString totalStr = QString::number(pb.qty * pb.price * VAT, 'f', 2) + " " + pb.curr;
        priceWidth = std::max(priceWidth, fm.horizontalAdvance(unitStr));
        totalWidth = std::max(totalWidth, fm.horizontalAdvance(totalStr));
    }
    priceWidth += 10;
    totalWidth += 10;
    int colX = rx;
    painter->drawText(QRect(colX, ry, qtyWidth, 16), Qt::AlignLeft, "Qty");
    colX += qtyWidth;
    painter->drawText(QRect(colX, ry, priceWidth, 16), Qt::AlignLeft, "Price");
    colX += priceWidth;
    painter->drawText(QRect(colX, ry, totalWidth, 16), Qt::AlignLeft, "Total");
    ry += 18;
    for (const auto& pb : part.priceBreaks) {
        QString qtyStr   = QString::number(pb.qty);
        QString unitStr  = QString::number(pb.price, 'f', 2) + " " + pb.curr;
        QString totalStr = QString::number(pb.price * pb.qty * VAT, 'f', 2) + " " + pb.curr;

        int colX = rx;
        painter->drawText(QRect(colX, ry, qtyWidth, 16), Qt::AlignLeft, qtyStr);
        colX += qtyWidth;
        painter->drawText(QRect(colX, ry, priceWidth, 16), Qt::AlignLeft, unitStr);
        colX += priceWidth;
        painter->drawText(QRect(colX, ry, totalWidth, 16), Qt::AlignLeft, totalStr);

        ry += 18;
        if (ry > bg.bottom() - 18) break; // отрезать лишнее
    }




*/