//FILE:  ui/mainwindow.cpp
#include "mainwindow.h"

#define LOG_ON

MainWindow::MainWindow(QWidget* parent):QWidget(parent){
    loadSettings(SSTR(CONFIG_FILENAME));//<-- macro to access settings variables
    setWindowTitle("Part Search");
    setWindowIcon(QIcon(SSTR(ICON_PARTSEARCH_PATH)));
    resize(SVAL(MAINWINDOW_SIZE_X), SVAL(MAINWINDOW_SIZE_Y));
    //======================== INITIALIZE ========================
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QHBoxLayout* topLayout = new QHBoxLayout;
    QHBoxLayout* bottomLayout = new QHBoxLayout;
    //QPushButton* importBtn = new QPushButton("Import to KiCAD");
    QComboBox*   combobox = new QComboBox;

    btnSettings = new QPushButton;
    debugOutputLabel = new QLabel;
    partModel = new PartModel(this);
    input = new QLineEdit;
    //searchButton = new QPushButton("Search");
    resultsView = new QListView(this);

    //=============================== LAYOUT ===============================
    updatePlaceholder();
    input->installEventFilter(this);

    combobox->setIconSize(QSize(SVAL(ICON_COMBOBOX_SIZE), SVAL(ICON_COMBOBOX_SIZE)));
    initSupplier(combobox); 

    btnSettings->setIcon(QIcon(SSTR(ICON_SETTINGS_PATH)));
    btnSettings->setIconSize(QSize(SVAL(ICON_SETTINGS_SIZE),SVAL(ICON_SETTINGS_SIZE)));
    btnSettings->setToolTip("Settings");
    btnSettings->setCheckable(true);

    topLayout->addWidget(input);
    //topLayout->addWidget(searchButton);
    topLayout->addWidget(combobox);
    topLayout->addWidget(btnSettings);
    mainLayout->addLayout(topLayout);

    resultsView->setModel(partModel);
    resultsView->setSelectionMode(QAbstractItemView::SingleSelection);
    resultsView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);             
    resultsView->setUniformItemSizes(false);        
    resultsView->setResizeMode(QListView::Adjust); 
    resultsView->setWordWrap(true);
    resultsView->setWrapping(false);
    

    mainLayout->addWidget(resultsView);

    debugOutputLabel->setStyleSheet("font-style: italic;");
    debugOutputLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
#ifdef LOG_ON
    qInstallMessageHandler(logHandler);
#endif

    bottomLayout->addWidget(debugOutputLabel);
    //bottomLayout->addWidget(importBtn);
    mainLayout->addLayout(bottomLayout);

    QScrollBar* bar = resultsView->verticalScrollBar();
    bar->setSingleStep(SVAL(SCROLL_STEP));

    input->setFocus();//set foscus to input field at start
    initSettings();
    mainLayout->insertWidget(1, scrollSettings); 



    //===================================== CONNECT ====================================
    //connect(cancelBtn, &QPushButton::clicked, [=]() { qInfo() << "Canceled"; });
    connect(bar, &QScrollBar::valueChanged, this, &MainWindow::onScrollChanged);
    //connect(searchButton, &QPushButton::clicked, this, &MainWindow::onSearch);
    connect(input, &QLineEdit::returnPressed, this, &MainWindow::onSearch);
    connect(resultsView, &QListView::activated, this, [this](const QModelIndex &index){
        if (!index.isValid()) return;
        exportData();
    });
    connect(btnSettings, &QPushButton::toggled, this, [this](bool checked){
        this->resultsView->setVisible(!checked);
        this->scrollSettings->setVisible(checked);
        
        checked ? this->adjustSize() : resize(SVAL(MAINWINDOW_SIZE_X), SVAL(MAINWINDOW_SIZE_Y));
        qInfo() << (checked ? "Settings Selected" : "Results List Selected"); 
        updatePlaceholder();

    });
    connect(combobox, QOverload<int>::of(&QComboBox::currentIndexChanged),this, [=](int index) {
        qDebug() << "MainWindow > Combobox switched to index:" << index;
        bar->blockSignals(true);
        ++m_requestEpoch;
        if (currentReply) { currentReply->abort(); currentReply->deleteLater(); currentReply = nullptr; }
        if (currentSupplier) { currentSupplier->deleteLater(); currentSupplier = nullptr; }
        resetSearch();
        selectSupplier(index);
        SVAL(SUPPLIER) = index;
        bar->blockSignals(false);
    });
}
MainWindow::~MainWindow() {
    resetSearch();
    delete currentSupplier;
    currentSupplier = nullptr;
    saveSettings(SSTR(CONFIG_FILENAME));
}
void MainWindow::initSupplier(QComboBox* combobox) {
    if (!combobox){ qWarning() << "Error at initSupplier(): combobox is null"; return; }
    combobox->blockSignals(true);
    combobox->clear();
    for (const auto& s : suppliers){ combobox->addItem(QIcon(s.iconPath), s.name, s.id); }
    int saved = SVAL(SUPPLIER);
    if (saved >= 0 && saved < static_cast<int>(suppliers.size())) {
        combobox->setCurrentIndex(saved);
        selectSupplier(saved);
    } else {
        combobox->setCurrentIndex(MOUSER);
        selectSupplier(MOUSER);
    }
    combobox->blockSignals(false);
}
void MainWindow::selectSupplier(int which){
    qInfo() << "Selected supplier: " << which ;
    switch (which) {
        case MOUSER:
            currentSupplier = new MouserSupplier(netMgr);
            break;
        case DIGIKEY:
            currentSupplier = new DigikeySupplier(netMgr);
            break;
        default:
            qWarning() << "initSupplier: unexpected supplier index, defaulting to Mouser";
            currentSupplier = new MouserSupplier(netMgr);
            break;
    }
}
void MainWindow::initSettings(){// initialize Settings Panel
    QWidget* settingsPanel = new QWidget(this);
    QVBoxLayout* settingsLayout = new QVBoxLayout(settingsPanel);
    settingsPanel->setLayout(settingsLayout);
    for (int i = 0; i < CONSTRAIN_VISIBLE_SETTINGS; ++i) { //generate settings lines
        if (i == EXPORT_FLAGS) { //create checkboxes
            QCheckBox* checkboxes[EXPORT_FIELDS_SIZE];
            QLabel* label = new QLabel("Fields to Export", settingsPanel);
            QFont f = label->font();
            f.setBold(true);
            f.setPointSize(f.pointSize() + 1);      // optional: make it slightly bigger
            label->setFont(f);
            label->setAlignment(Qt::AlignCenter);

            // add top separator
            QFrame* sepTop = new QFrame(settingsPanel);
            sepTop->setFrameShape(QFrame::HLine);
            sepTop->setFrameShadow(QFrame::Sunken);
            sepTop->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            settingsLayout->addWidget(sepTop);
            settingsLayout->addWidget(label);
            const int cols = 3; // change: 2,3,4... how many columns you want
            QWidget* gridContainer = new QWidget(settingsPanel);
            QGridLayout* grid = new QGridLayout(gridContainer);
            // tighten spacing/margins to be compact
            grid->setSpacing(6);                 // gap between checkboxes
            grid->setContentsMargins(4, 2, 4, 2);
            for (int j = 0; j < EXPORT_FIELDS_SIZE; ++j) {
                checkboxes[j] = new QCheckBox(exportFields[j].name, gridContainer);
                // make checkboxes small/compact
                QFont f = checkboxes[j]->font();
                f.setPointSize(f.pointSize() - 1);       // slightly smaller text
                checkboxes[j]->setFont(f);
                checkboxes[j]->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
                // set initial checked state from settings
                checkboxes[j]->setChecked((SVAL(EXPORT_FLAGS) & (1 << j)) != 0);
                // place into grid
                int row = j / cols;
                int col = j % cols;
                grid->addWidget(checkboxes[j], row, col);
                // update settings flag when toggled
                connect(checkboxes[j], &QCheckBox::toggled, this, [this, j](bool checked){
                    if (checked) SVAL(EXPORT_FLAGS) |= (1 << j);
                    else          SVAL(EXPORT_FLAGS) &= ~(1 << j);
                });
            }
            // add the grid widget to your main settings layout
            settingsLayout->addWidget(gridContainer);
            QFrame* sepBottom = new QFrame(settingsPanel);
            sepBottom->setFrameShape(QFrame::HLine);
            sepBottom->setFrameShadow(QFrame::Sunken);
            sepBottom->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            settingsLayout->addWidget(sepBottom);
            continue;
        }
        QTextEdit* te = new QTextEdit;
        QLabel* label = new QLabel(QString(SNM(i)));
        QHBoxLayout* line = new QHBoxLayout;
        te->setText(!SVAL(i) ? SSTR(i) : QString::number(SVAL(i)));
        te->setFixedHeight(SVAL(SETTINGS_TEXTBOX_SIZE_Y));
        line->addWidget(label);
        line->addWidget(te);
        settingsLayout->addLayout(line);
        connect(te, &QTextEdit::textChanged, this, [=]() {
            QString text = te->toPlainText();
            bool ok = false;
            int num = text.toInt(&ok);
            if (ok) {
                settings[i].val = num;
                settings[i].str.clear();
            } else {
                settings[i].str = text;
                settings[i].val = 0;
            }
            qDebug() << "Updated setting" << settings[i].name
                    << "to" << (ok ? QString::number(num) : text);
        });
    }
    settingsLayout->addStretch();
    scrollSettings = new QScrollArea(this);
    scrollSettings->setWidget(settingsPanel);
    scrollSettings->setWidgetResizable(true);
    scrollSettings->setVisible(false);
}
void MainWindow::resetSearch() {
    ++m_requestEpoch; // invalidate older replies
    offset = 0;
    resultssize = 0;
    noResults = false;
    firstReq = true;
}
void MainWindow::onSearch() {
    if (scrollSettings->isVisible()) {
        scrollSettings->setVisible(false);
        resultsView->setVisible(true); 
        resize(SVAL(MAINWINDOW_SIZE_X), SVAL(MAINWINDOW_SIZE_Y));
        btnSettings->blockSignals(true);
        btnSettings->setChecked(false);  
        btnSettings->blockSignals(false);
    }
    QString kw = input->text().trimmed();
    if (kw.isEmpty()) return;

    currentKeyword = kw;
    resetSearch();
    qInfo() << "Searching for:" << currentKeyword;

    fetchPartsAsync(currentSupplier, currentKeyword, false);
    input->setFocus();
}
void MainWindow::updatePlaceholder() {
    if (SSTR(API_KEY_MOUSER).length()   < 35  ||
       ((SSTR(DIGIKEY_CLSECRET).length() < 47) && (SSTR(DIGIKEY_CLIENTID).length() < 63))) {
        apierror = true;
        input->setPlaceholderText("Provide correct API first");
    } else {
        apierror = false;
        input->setPlaceholderText("Provide keyword or part number and press <Enter>");
    }
}
void MainWindow::fetchPartsAsync(PartSupplier* supplier,const QString& keyword,bool prepend = false){ //from network request to results list
    if (prepend) { offset -= resultssize; }
    else         { offset += resultssize; }
    if (offset < 0) offset = 0;
    //qDebug() << "fetchPartsAsync > Fetching parts...";
    const quint64 epoch = m_requestEpoch;
    QPointer<PartSupplier> sup = supplier;// becomes null if deleted
    if (!sup) {qWarning() << "Error at fetchPartsAsync(): supplier is nullptr"; return; }
    QByteArray payload;
    //qDebug() << "keyword=" << keyword  << " offset=" << offset  << " payload=" << payload;
    QNetworkRequest req = supplier->searchRequest(keyword, SVAL(REQUEST_LIMIT), offset, payload);
    QNetworkReply* reply = netMgr.post(req, payload);
    if(!reply) { qWarning() << "Error at fetchPartsAsync(): Network reply is nullptr"; return; } 
    else       { currentReply = reply; /*qDebug() << "currentReply = " << currentReply; */ }
    connect(reply, &QNetworkReply::finished, this,
        [this, reply, sup, epoch, prepend]() {
        qDebug() << "fetchPartsAsync > QNetworkReply finished";
        if (epoch != m_requestEpoch || reply != currentReply || !sup || sup != currentSupplier) { reply->deleteLater(); return; }
        
        QByteArray resp = reply->readAll(); 
        reply->deleteLater();
        
        if (reply == currentReply) currentReply = nullptr;

        int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(); qInfo() << "HTTP status:" << status;
        
        if (status == 0 || reply->error() != QNetworkReply::NoError) { qWarning() << "Network error:" << reply->errorString(); return; }
        if (firstReq) { total = sup->totalFromJson(resp);qInfo() << "Total available parts:" << total; firstReq = false; }

        qInfo() << "Parsing results...";
    
        const QList<QJsonObject> results = sup->parseResults(resp); //qInfo() << "First result:" << results.at(0);
        
        QList<PartData> newParts;
        
        resultssize = results.size();
        if(resultssize == 0){ noResults = true; qInfo() << "No more results"; return; }

        newParts.reserve(resultssize);
        for (const auto& part : results) { newParts.append(sup->toPartData(part)); }
        if (newParts.isEmpty()) { noResults = true; return; }
        const int prevCount = partModel->rowCount();
        qDebug() << "prevCount:" << prevCount;

        if (prepend) {
            partModel->prependParts(newParts);
            createCards(newParts, 0, sup, epoch, /*scroll to*/ newParts.size());
            qDebug() << "Part card prepended";
        } else {
            partModel->addParts(newParts);
            createCards(newParts, prevCount, sup, epoch, prevCount);
            qDebug() << "New part card added";
        }
        
        resultsView->doItemsLayout(); qDebug()  << "fetchPartsAsync > doItemsLayout";
        resultsView->updateGeometry(); qDebug() << "fetchPartsAsync > updateGeometry";

        if (prepend) { 
            qDebug() << "scroll back after prepend...";  //problem was that it jumps to first
            const int newAnchorRow = newParts.size();
            QModelIndex newAnchor = partModel->index(newAnchorRow, 0);
            if (newAnchor.isValid()) {
                resultsView->scrollTo(newAnchor, QAbstractItemView::PositionAtTop);
                if (auto* bar = resultsView->verticalScrollBar())
                    bar->setValue(bar->value());
            }
        }else if(cardCreateFinished){ //jump to first newly added part
            cardCreateFinished = false;
            const int newAnchorRow = prevCount; // first new part index
            QModelIndex newAnchor = partModel->index(newAnchorRow, 0);
            if (newAnchor.isValid()) {
                resultsView->scrollTo(newAnchor, QAbstractItemView::PositionAtTop);
            }
        }
        
        qInfo() << "Loaded" << offset+resultssize << "parts of" << total 
                << (show_tip ? "| Move by '↑','↓' and press <Enter> to select. <Tab> to search again.":"");
        if(show_tip){show_tip = false;}
#ifdef SAVE_RESPONSE //save json response for debugging
        qInfo() << "Response body:" << resp;
        QFile file("response.txt");
        if (file.open(QIODevice::WriteOnly)) {
            file.write(resp);
            file.close();
            qInfo() << "Response saved to response.txt";
        } else {
            qWarning() << "Cannot open file for writing:" << file.errorString();
        }
#endif
    });
}
void MainWindow::createCards(const QList<PartData>& parts, int startRow, QPointer<PartSupplier> supplier, quint64 epoch, int scrollToRow){
    // do widget creation slightly later (safe), but handle scrolling AFTER layout
    QTimer::singleShot(0, this, [this, parts, startRow, supplier, epoch, scrollToRow]() {
        if (epoch != m_requestEpoch) {
            qWarning() << "createCards aborted: epoch mismatch";
            return;
        }
        if (!supplier) {
            qWarning() << "createCards: supplier already deleted — abort creating cards";
            return;
        }

        for (int i = 0; i < parts.size(); ++i) {
            QModelIndex idx = partModel->index(startRow + i, 0);
            if (!idx.isValid()) continue;
            QWidget* card = createPartCard(parts.at(i), supplier, resultsView);
            if (!card) continue;
            card->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
            card->setMinimumHeight(SVAL(IMG_RESULT_SIZE_Y) + SVAL(CARD_MARGIN));
            resultsView->setIndexWidget(idx, card);
        }

        resultsView->doItemsLayout();
        resultsView->updateGeometry();

        // If requested, scroll to the requested row (first newly added part)
        if (scrollToRow >= 0) {
            QTimer::singleShot(0, this, [this, scrollToRow]() {
                QModelIndex anchor = partModel->index(scrollToRow, 0);
                if (anchor.isValid()) {
                    resultsView->scrollTo(anchor, QAbstractItemView::PositionAtTop);
                    if (auto *sel = resultsView->selectionModel()) {
                        sel->setCurrentIndex(anchor, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
                    }
                    resultsView->setFocus();
                } else {
                    qWarning() << "createCards: anchor index invalid for scrollToRow =" << scrollToRow;
                }
            });
        }
    });
}
QWidget* MainWindow::createPartCard(const PartData& part, QPointer<PartSupplier> supplier, QWidget* parent) {
    QWidget* card = new QWidget(parent);

    auto hCard = new QHBoxLayout(card);
    // ............................... Image ...............................
    QLabel* image = new QLabel;
    image->setFixedSize(SVAL(IMG_RESULT_SIZE_X), SVAL(IMG_RESULT_SIZE_Y));
    hCard->addWidget(image);
    if (!part.imgUrl.isEmpty()) supplier->fetchImageIntoWidget(part.imgUrl, image); 
    // ............................... Left column ...............................
    auto leftCol = new QVBoxLayout;
    leftCol->addWidget(selectableLabel(QString("<b>Part No:</b> %1").arg(part.prtnm), true));
    leftCol->addWidget(selectableLabel(QString("<b>Mfr. No:</b> %1").arg(part.mfrno), true));
    leftCol->addWidget(selectableLabel(QString("<b>Mfr.:</b> %1").arg(part.mfr), true));
    QTextEdit* descrEdit = new QTextEdit;
    descrEdit->setReadOnly(true);
    descrEdit->setHtml(QString("<b>Description:</b><br>%1").arg(part.descr));
    descrEdit->setFixedHeight(SVAL(DESCR_FIELD_SIZE));
    leftCol->addWidget(descrEdit);
    auto datasheetLabel = new QLabel(QString("<b>Datasheet:</b> <a href='%1'>Link</a>").arg(part.dsUrl));
    datasheetLabel->setTextFormat(Qt::RichText);
    datasheetLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    datasheetLabel->setOpenExternalLinks(true);
    leftCol->addWidget(datasheetLabel);
    leftCol->addStretch();
    leftCol->setContentsMargins(0,0,0,0);
    hCard->addLayout(leftCol, 1);
    // ...................... Prices, Stock ................................
    auto rightCol = new QVBoxLayout;
    rightCol->setContentsMargins(0,0,0,0);
    QString availText = QString("<b>Availability:</b> %1").arg(part.avail);
    QLabel* availLabel = selectableLabel(availText, true);
    if (part.avail.contains("None", Qt::CaseInsensitive) ||
        part.avail.contains("Non-Stocked", Qt::CaseInsensitive) ||
        part.avail == "0"){
        availLabel->setStyleSheet("color: red");
    } else {
        availLabel->setStyleSheet("color: #00FF14");
        
    }
    rightCol->addWidget(availLabel);
    // .................... Prices header, names ...........................
    auto hdr = new QHBoxLayout;
    hdr->addWidget(selectableLabel("<b>Quantity</b>", true));
    hdr->addWidget(selectableLabel("<b>Unit Price</b>", true));
    hdr->addWidget(selectableLabel(QString("<b>Total+VAT%1%</b>").arg(float(SVAL(VAT)), 0, 'f', 0), true));
    rightCol->addLayout(hdr);
    // ................... Rows of Prices(quantity) .........................
    auto pricesContainer = new QWidget;
    auto pricesLayout = new QVBoxLayout(pricesContainer);
    pricesLayout->setContentsMargins(0,0,0,0);
    pricesLayout->setSpacing(4);

    for (const auto &pb : part.breaks) {
        float ext = pb.price * pb.qty * float(SVAL(VAT)+100)*0.01;
        auto row = new QHBoxLayout;
        row->addWidget(selectableLabel(QString::number(pb.qty)));
        row->addWidget(selectableLabel(QString(" %1 %2").arg(QString::number(pb.price, 'f', 2), pb.curr)));
        row->addWidget(selectableLabel(QString(" %1 %2").arg(QString::number(ext, 'f', 2), pb.curr)));
        pricesLayout->addLayout(row);
    }

    auto scrollPrices = new QScrollArea;
    scrollPrices->setWidget(pricesContainer);
    scrollPrices->setWidgetResizable(true);
    scrollPrices->setFixedHeight(SVAL(PRICE_FIELD_SIZE));
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
}// ############################################ FUNCTION END ################################################################
void MainWindow::onScrollChanged(int value){//lazy scroll
    QScrollBar* bar = resultsView->verticalScrollBar();
    if (!bar) return;
    //qDebug() << "Scrollbar:" << value << " of " << bar->maximum();
    // ============================= Lazy Scroll Down =============================
    if (value >= bar->maximum() - SVAL(SCROLL_STEP) && !currentReply && !noResults) {
        qInfo() << "Loading new results...";
        fetchPartsAsync(currentSupplier, currentKeyword, false);
        while (partModel->rowCount() > SVAL(LIMIT_RESULTS)) { //optimization
            killRowWidget(0);
            partModel->removeFirst(); //clear previous results
    }   }
    // =============================== Lazy Scroll Up ==============================
    if (value <= SVAL(SCROLL_STEP) && offset > 0 && !currentReply) {
        qInfo() << "Loading previous results...";
        fetchPartsAsync(currentSupplier, currentKeyword, true);
        while (partModel->rowCount() > SVAL(LIMIT_RESULTS)) { //optimization
            int last = partModel->rowCount() - 1; //clear previous results
            killRowWidget(last);
            partModel->removeLast();
    }   }
}
void MainWindow::killRowWidget(int row) {
    if (!resultsView || row < 0 || row >= partModel->rowCount()){ return; }
    QModelIndex idx = partModel->index(row, 0);
    if (!idx.isValid()){ return; }
    QWidget* w = resultsView->indexWidget(idx);
    if (w) { resultsView->setIndexWidget(idx, nullptr);
             w->deleteLater(); }
}
void MainWindow::killAllWidgets() { //remove old cards for optimization
    if (!resultsView) return;
    for (int i = partModel->rowCount() - 1; i >= 0; --i) {
        killRowWidget(i);
    }
}
bool MainWindow::eventFilter(QObject* obj, QEvent* event) { //control by arrows on keyboard
    if (obj == input && event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Up || keyEvent->key() == Qt::Key_Down) {
            if (resultsView->model()->rowCount() > 0) {
                resultsView->setFocus();
                QModelIndex current = resultsView->currentIndex();
                if (!current.isValid()) {
                    QModelIndex first = resultsView->model()->index(0, 0);
                    if (first.isValid()) {
                        resultsView->setCurrentIndex(first);
                        if (auto *sel = resultsView->selectionModel()) {
                            sel->setCurrentIndex(first, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows); }
                        resultsView->setFocus();
            }   }   }
        return true;
    }   }
    return QWidget::eventFilter(obj, event);
}
void MainWindow::exportData() {
    qInfo() << "Importing part to KiCAD...";
    QModelIndex idx = resultsView->currentIndex();
    if (!idx.isValid()) { qWarning() << "Error at exportData(): No part selected for export"; return; }
    QVariant v = partModel->data(idx, Qt::UserRole);
    if (!v.isValid()) { qWarning() << "Error at exportData(): Invalid part data at index:" << idx; return; }
    PartData part = v.value<PartData>();
    // Create JSON object
    QJsonObject obj;
    if (SVAL(EXPORT_FLAGS) & (1 << EID(PRTNM))){ obj["prtnm"] = part.prtnm; }
    if (SVAL(EXPORT_FLAGS) & (1 << EID(MFRNO))){ obj["mfrno"] = part.mfrno; }
    if (SVAL(EXPORT_FLAGS) & (1 << EID(MFR)))  { obj["mfr"]   = part.mfr;   }
    if (SVAL(EXPORT_FLAGS) & (1 << EID(DESCR))){ obj["descr"] = part.descr; }
    if (SVAL(EXPORT_FLAGS) & (1 << EID(AVAIL))){ obj["avail"] = part.avail; }
    if (SVAL(EXPORT_FLAGS) & (1 << EID(PRURL))){ obj["prUrl"] = part.prUrl; }
    if (SVAL(EXPORT_FLAGS) & (1 << EID(DSURL))){ obj["dsUrl"] = part.dsUrl; }
    if (SVAL(EXPORT_FLAGS) & (1 << EID(SUPPL))){ obj["suppl"] = part.suppl; }
    if((SVAL(EXPORT_FLAGS) & (1 << EID(QTY))) | (SVAL(EXPORT_FLAGS)  & (1 << EID(PRICE)))) {
        QJsonArray breaks; //\--- skip this part if 0 bits ---\//
        for (const auto& pb : part.breaks) {
            QJsonObject b;
            if (SVAL(EXPORT_FLAGS) & (1 << EID(QTY)))  { b["qty"]   = pb.qty;  }
            if (SVAL(EXPORT_FLAGS) & (1 << EID(PRICE))){ b["price"] = pb.price;
                                                         b["curr"]  = pb.curr; }
            breaks.append(b);
        }
        obj["priceBreaks"] = breaks;
    }
    QJsonDocument doc(obj);
    // --------------------- stdout -------------------------
    qInfo().noquote() << "Export[" << doc.toJson(QJsonDocument::Compact) << "]";
    // ---------------------- file --------------------------
    QFile file("export.json");
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
        qInfo() << "Part exported to export.json";
    } else { qWarning() << "Error at exportData(): Cannot open export.json for writing:" << file.errorString();}
    this->close(); //close window after export
}
void MainWindow::saveSettings(const QString &filename) {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Cannot open" << filename << "for writing";
        return;
    }
    QTextStream out(&file);
    for (int i = 0; i < SETTING_COUNT; ++i) {
        out << settings[i].name
            << "=int:" << settings[i].val
            << ";str:" << settings[i].str << "\n";
    }
}
void MainWindow::loadSettings(const QString &filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) { qWarning() << "Config file not found, using defaults"; return;}
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty() || line.startsWith("#")) continue;
        QStringList kv = line.split('=');
        if (kv.size() != 2) continue;
        QString key = kv[0].trimmed();
        QString value = kv[1].trimmed();
        for (int i = 0; i < SETTING_COUNT; ++i) {
            if (settings[i].name == key) {
                QStringList parts = value.split(';'); //expected "val:123;str:abc"
                for (auto &p : parts) {
                    if (p.startsWith("int:"))     {settings[i].val = p.mid(4).toInt();} 
                    else if (p.startsWith("str:")){settings[i].str = p.mid(4);        }
                }
                break;
            }
        }
    }
}

