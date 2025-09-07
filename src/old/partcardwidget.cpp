// partcardwidget.cpp
#include "partcardwidget.h"
#include <QHBoxLayout>
#include <QScrollArea>


static QLabel* makeLabel(const QString& text, bool bold = false) {
    QLabel* lbl = new QLabel(text);
    QFont f = lbl->font();
    f.setBold(bold);
    lbl->setFont(f);
    lbl->setTextInteractionFlags(Qt::TextSelectableByMouse);
    lbl->setWordWrap(true);
    return lbl;
}
PartCardWidget::PartCardWidget(const PartData& part, QWidget* parent)
    : QWidget(parent)
{
    auto hMain = new QHBoxLayout(this);
    hMain->setContentsMargins(4,4,4,4);
    hMain->setSpacing(8);

    // === левая колонка: иконка + информация ===
    auto thumb = new QLabel;
    thumb->setFixedSize(80,80);
    if (!part.image.isNull())
        thumb->setPixmap(part.image.scaled(80,80,Qt::KeepAspectRatio,Qt::SmoothTransformation));
    hMain->addWidget(thumb);

    auto leftCol = new QVBoxLayout;
    leftCol->addWidget(selectableLabel(QString("<b>Mouser No:</b> %1").arg(part.partNumber), true));
    leftCol->addWidget(selectableLabel(QString("<b>Mfr. No:</b> %1").arg(part.manufacturer), true));
    leftCol->addWidget(selectableLabel(QString("<b>Mfr.:</b> %1").arg(part.description), true));

    auto ds = new QLabel(QString("<a href='%1'>Datasheet</a>").arg(part.datasheetUrl));
    ds->setTextFormat(Qt::RichText);
    ds->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ds->setOpenExternalLinks(true);
    leftCol->addWidget(ds);
    leftCol->addStretch();
    hMain->addLayout(leftCol, /*stretch=*/2);

    // === правая колонка — в скролле ===
    QWidget* priceContainer = new QWidget;
    auto vPrices = new QVBoxLayout(priceContainer);
    vPrices->setContentsMargins(0,0,0,0);
    vPrices->setSpacing(2);

    // заголовок
    const int ROW_HEIGHT = 20;
    auto hdrWidget = new QWidget;
    hdrWidget->setFixedHeight(ROW_HEIGHT);
    auto hdr = new QHBoxLayout(hdrWidget);
    hdr->setContentsMargins(0,0,0,0);
    hdr->addWidget(makeLabel("Qty", true));
    hdr->addSpacing(10);
    hdr->addWidget(makeLabel("Unit", true));
    hdr->addSpacing(10);
    hdr->addWidget(makeLabel("Total", true));
    hdr->addStretch();
    vPrices->addWidget(hdrWidget);

    // все строки
    const int MAX_VISIBLE = 2;
    for (auto &pb : part.priceBreaks) {
        float total = pb.price * pb.qty * VAT;
        auto rowWidget = new QWidget;
        rowWidget->setFixedHeight(ROW_HEIGHT);
        auto row = new QHBoxLayout(rowWidget);
        row->setContentsMargins(0,0,0,0);
        row->addWidget(makeLabel(QString::number(pb.qty)));
        row->addSpacing(10);
        row->addWidget(makeLabel(
            QString("%1 %2").arg(QString::number(pb.price,'f',2), pb.curr)));
        row->addSpacing(10);
        row->addWidget(makeLabel(
            QString("%1 %2").arg(QString::number(total,'f',2), pb.curr)));
        row->addStretch();
        vPrices->addWidget(rowWidget);
    }
    vPrices->addStretch();

    // сам QScrollArea
    auto scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scroll->setWidget(priceContainer);

    // жестко задаём высоту: заголовок + MAX_VISIBLE строк + немного отступа
    scroll->setFixedHeight(ROW_HEIGHT * (MAX_VISIBLE + 1) + 4);

    hMain->addWidget(scroll, /*stretch=*/1);
}

/*
QWidget* MouserSupplier::createPartCard(const QJsonObject &part) {
    // Extract fields
    QString msrno = getField(part, "MouserPartNumber");
    QString avail = getField(part, "Availability", "Non-Stocked");
    auto [order, ordqnt, orddate] = parseAvailabilityOnOrder(part);
    QString curr  = getField(part, "Currency", "_");
    QString mfrno = getField(part, "ManufacturerPartNumber");
    QString mfr   = getField(part, "Manufacturer");
    QString descr = getField(part, "Description");
    QString dsUrl = getField(part, "DataSheetUrl",
                     getField(part, "ProductDetailUrl", QString()));
    QList<PriceBreak> breaks = parsePriceBreaks(part);
    // ................... Main shell of result card ....................
    QWidget* card = new QWidget;
    auto hCard = new QHBoxLayout(card);

    // ........................ Image + Info  ...........................
    QLabel* image = new QLabel;
    image->setFixedSize(IMG_RESULT_SIZE_X, IMG_RESULT_SIZE_Y);
    hCard->addWidget(image);

    // ......................... Load image .............................
    QString imgPath = part.value("ImagePath").toString();
    if (!imgPath.isEmpty()) {
        if (imgPath.startsWith('/')) imgPath.prepend("https://www.mouser.com");
        fetchImageIntoWidget(imgPath, image);
    }

    auto leftCol = new QVBoxLayout;
    leftCol->addWidget(selectableLabel(QString("<b>Mouser No:</b> %1").arg(msrno), true));
    leftCol->addWidget(selectableLabel(QString("<b>Mfr. No:</b> %1").arg(mfrno), true));
    leftCol->addWidget(selectableLabel(QString("<b>Mfr.:</b> %1").arg(mfr), true));
    leftCol->addWidget(selectableLabel(QString("<b>Description:</b><br>%1").arg(descr), true));

    auto datasheetLabel = new QLabel(QString("<b>Datasheet:</b> <a href='%1'>Link</a>").arg(dsUrl));
    datasheetLabel->setTextFormat(Qt::RichText);
    datasheetLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    datasheetLabel->setOpenExternalLinks(true);
    leftCol->addWidget(datasheetLabel);

    QPushButton* importBtn = new QPushButton("Import to KiCAD");
    QObject::connect(importBtn, &QPushButton::clicked, [=]() {
        qInfo() << "Importing part to KiCAD:" << mfrno;
    });
    leftCol->addWidget(importBtn);
    leftCol->addStretch();
    hCard->addLayout(leftCol, 1);

    // ...................... Prices, Stock, Order ...........................
    auto rightCol = new QVBoxLayout;
    QString availText = QString("<b>Availability:</b> %1").arg(avail);
    if (ordqnt != "0")
        availText += QString(" (%1 expected %2)").arg(ordqnt, orddate);
    QLabel* availLabel = selectableLabel(availText, true);
    availLabel->setStyleSheet(avail.contains("In Stock", Qt::CaseInsensitive)
                              ? "color: #00FF14" : "color: red");
    rightCol->addWidget(availLabel);

    // .................... Prices header, names ...........................
    auto hdr = new QHBoxLayout;
    hdr->addWidget(selectableLabel("<b>Quantity</b>", true));
    hdr->addWidget(vPricesSeparator());
    hdr->addWidget(selectableLabel("<b>Unit Price</b>", true));
    hdr->addWidget(vPricesSeparator());
    hdr->addWidget(selectableLabel(
        QString("<b>Total+VAT%1%</b>")
            .arg((VAT - 1.0) * 100.0, 0, 'f', 0), true));
    rightCol->addLayout(hdr);

    // ................... Rows of Prices(quantity) .........................
    for (const auto &pb : breaks) {
        float ext = pb.price * pb.qty * VAT;
        auto row = new QHBoxLayout;
        row->addWidget(selectableLabel(QString::number(pb.qty)));
        row->addWidget(vPricesSeparator(1,"#444"));
        row->addWidget(selectableLabel(QString("%1 %2")
                                           .arg(QString::number(pb.price, 'f', 2), pb.curr)));
        row->addWidget(vPricesSeparator(1,"#444"));
        row->addWidget(selectableLabel(QString("%1 %2")
                                           .arg(QString::number(ext, 'f', 2), pb.curr)));
        rightCol->addLayout(row);
    }
    rightCol->addStretch();

    // ......................... Bold vertical line .............................
    QFrame* vline = new QFrame;
    vline->setFrameShape(QFrame::VLine);
    vline->setFrameShadow(QFrame::Sunken);
    vline->setStyleSheet("background-color: #6C6C6C;");
    hCard->addWidget(vline);

    hCard->addLayout(rightCol, 1);
    return card;
}

*/