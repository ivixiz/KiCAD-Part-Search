//FILE main.cpp
#include "main.h"

#define LOG_ON

float                  VAT            = 0.23f + 1.0f;
int                    REQUEST_SIZE   = 30; //min 30 for Mouser, 10 faster for DigiKey
int                    MAX_RESULTS    = 10;


QNetworkAccessManager* globalNetMgr   = nullptr;
QNetworkReply*         currentReply   = nullptr;
QPushButton*           searchButton   = nullptr;
//QVBoxLayout*         resultsLayout  = nullptr;
QListWidget*           resultsList    = nullptr;
int                    currentOffset  = 0;
int                    currentLoaded  = 0;
QString                currentKeyword;
bool                   noResults      = false;
bool                   firstReq       = true;

int main(int argc, char** argv) {
    
    QApplication a(argc,argv);
    QNetworkAccessManager mgr;
    globalNetMgr = &mgr;

    MainWindow mainwindow;

    QWidget w; w.setWindowTitle("Attribute Search");

    //PartSupplier* currentSupplier = new MouserSupplier();
    PartSupplier* currentSupplier = new DigikeySupplier();

    QVBoxLayout* main = new QVBoxLayout(&w);
    QHBoxLayout* top = new QHBoxLayout;
    auto input = new QLineEdit; input->setPlaceholderText("Enter keyword or part number");
    auto btn_search = new QPushButton("Search"); searchButton = btn_search;
    auto btn_settings = new QPushButton;
         btn_settings->setIcon(QIcon(ICON_SETTINGS_PATH));
         btn_settings->setIconSize(QSize(20,20));
         btn_settings->setToolTip("Settings");     
    top->addWidget(input); top->addWidget(btn_search); top->addWidget(btn_settings);
    main->addLayout(top);

    auto listWidget = new QListWidget;       
    listWidget->setViewMode(QListView::ListMode);
    listWidget->setResizeMode(QListView::Adjust);
    listWidget->setSelectionMode(QAbstractItemView::NoSelection);
    listWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    listWidget->setSpacing(6);
    main->addWidget(listWidget);

    resultsList = listWidget;

    debugOutputLabel = new QLabel;
    debugOutputLabel->setStyleSheet("font-style: italic;");
    debugOutputLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
#ifdef LOG_ON
    qInstallMessageHandler(logHandler);
#endif
    //main->addWidget(debugOutputLabel);

    QHBoxLayout* bott = new QHBoxLayout;
    QPushButton* importBtn = new QPushButton("Import to KiCAD");
    QObject::connect(importBtn, &QPushButton::clicked, [=]() {
        qInfo() << "Importing part to KiCAD";
    });
    QPushButton* cancel = new QPushButton("Cancel");

    QObject::connect(cancel, &QPushButton::clicked, [=]() {
        qInfo() << "Canceled";
    });
    bott->addWidget(debugOutputLabel); 
    bott->addWidget(importBtn); 
    bott->addWidget(cancel); 
    main->addLayout(bott);



    //.................... Search ...................
    QObject::connect(btn_search,&QPushButton::clicked,[&](){
        QString kw = input->text().trimmed();
        if(kw.isEmpty()) return;
        currentKeyword = kw;
        currentOffset = 0;
        currentLoaded = 0;
        noResults = false;
        firstReq  = true;
        // ....... Clear old widgets .............
        resultsList->clear();
        qInfo() << "Awaiting for response...";
        mainwindow.fetchPartsAsync(currentSupplier,currentKeyword, currentOffset,false);
    });
    QObject::connect(input,&QLineEdit::returnPressed, btn_search, &QPushButton::click);

    //.................................. Lazy scroll ....................................
    auto bar = resultsList->verticalScrollBar();
    bar->setSingleStep(10);
    QObject::connect(resultsList->verticalScrollBar(), &QScrollBar::valueChanged, [&](int v){     
        if(v >= bar->maximum()-10 && !currentReply && !noResults) {
            qInfo() << "Loading more results...";
            currentOffset += REQUEST_SIZE;
            mainwindow.fetchPartsAsync(currentSupplier,currentKeyword,currentOffset,false);
            while (resultsList->count() > MAX_RESULTS) {
                QListWidgetItem* item = resultsList->takeItem(0);
                delete resultsList->itemWidget(item);
                delete item;
            }
        }
        if (v <= 10 && currentOffset > 0 && !currentReply) {
            qInfo() << "Loading previous results...";
            currentOffset -= REQUEST_SIZE;
            if (currentOffset < 0) currentOffset = 0;
            mainwindow.fetchPartsAsync(currentSupplier, currentKeyword, currentOffset,true);
            while (resultsList->count() > MAX_RESULTS) {
                int lastIndex = resultsList->count() - 1;
                QListWidgetItem* item = resultsList->takeItem(lastIndex);
                delete resultsList->itemWidget(item);
                delete item;
            }
        }
    });
    w.resize(800,600);
    w.show();

    return a.exec();
}



















// static void fetchMouserAsync(const QString& keyword, int offset)  {
//     if (currentReply && currentReply->isRunning()) {
//         currentReply->abort();
//         currentReply->deleteLater();
//     }

//     QString request = QString(R"({
//         "SearchByKeywordRequest": {
//             "apiKey": "%1",
//             "keyword": "%2",
//             "records": 30,
//             "startingRecord": %3,
//             "IncludeSearchResultsImages": true
//         }
//     })").arg(API_KEY_MOUSER, keyword).arg(offset + 1); // Mouser API is 1-based index

//     QNetworkRequest hd(QUrl("https://api.mouser.com/api/v1/search/keyword?apiKey=" + QString(API_KEY_MOUSER)));
//     hd.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

// #ifdef LOCK_AWAIT_RESPONSE
//     searchButton->setEnabled(false);
// #endif
//     QNetworkReply* reply = globalNetMgr->post(hd, request.toUtf8());
//     currentReply = reply;

//     QObject::connect(reply, &QNetworkReply::finished, [reply]() {
//         QByteArray resp = reply->readAll();
// #ifdef LOCK_AWAIT_RESPONSE
//         if (searchButton) searchButton->setEnabled(true);
// #endif
//         reply->deleteLater();
//         qDebug() << "Response = " << resp;
//         if (currentReply == reply) currentReply = nullptr;

//         appendResults(resp);
//     });
// }// ############################################ FUNCTION END ################################################################

