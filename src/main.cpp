//FILE main.cpp
#include "main.h"


int main(int argc, char** argv) {
    QApplication a(argc,argv);
    qRegisterMetaType<PartData>("PartData");
    
    MainWindow w; //go to ui/mainwindow.cpp
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

