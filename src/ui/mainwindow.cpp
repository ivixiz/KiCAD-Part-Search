//FILE:  ui/mainwindow.cpp
#include "mainwindow.h"


void MainWindow::fetchPartsAsync(PartSupplier* supplier, const QString& keyword, int offset, bool dir) {
    if (currentReply && currentReply->isRunning()) {
        currentReply->abort();
        currentReply->deleteLater();
    }
    QByteArray payload;
    //qInfo() << "Requesting...";
    QNetworkRequest req = supplier->searchRequest(keyword, offset, payload); // JSON request
    currentReply = globalNetMgr->post(req, payload); // get reply
    //qInfo() << "currentReply == " << currentReply;
    QObject::connect(currentReply, &QNetworkReply::finished, [=]() {                     qInfo() << "Request received";
        QVariant st = currentReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        int status = st.isValid() ? st.toInt() : -1;                                     qInfo() << "HTTP status:" << status;

        if (currentReply->error() != QNetworkReply::NoError) { qWarning() << "Network error:" << currentReply->errorString(); }
        QByteArray resp = currentReply->readAll();                                       qInfo() << "Updating current reply...";

        currentReply->deleteLater(); 
        currentReply = nullptr;

        static int total = 0;
        if(firstReq){total = supplier->totalFromJson(resp); firstReq = false;}           qInfo() << "Parsing results...";

        QList<QJsonObject> results = supplier->parseResults(resp);
        int pageLoaded = results.size();

        int oldValue = resultsList->verticalScrollBar()->value();
        int oldMax   = resultsList->verticalScrollBar()->maximum();

        if (!dir) {
            // scrolling down → append in natural order
            for (const auto& part : results) {
                QWidget* card = supplier->createPartCard(part);
                auto item = new QListWidgetItem;
                item->setSizeHint(card->sizeHint());
                resultsList->addItem(item);
                resultsList->setItemWidget(item, card);
            }
        } else {
            // scrolling up → prepend in reverse order
            for (int i = results.size() - 1; i >= 0; --i) {
                QWidget* card = supplier->createPartCard(results[i]);
                auto item = new QListWidgetItem;
                item->setSizeHint(card->sizeHint());
                resultsList->insertItem(0, item);
                resultsList->setItemWidget(item, card);
            }

            // keep scroll position stable
            int newMax = resultsList->verticalScrollBar()->maximum();
            int delta  = newMax - oldMax;
            resultsList->verticalScrollBar()->setValue(oldValue + delta);
        }

        qInfo() << "Loaded"     << pageLoaded 
                << "this page," << (dir ? currentLoaded -= pageLoaded : currentLoaded += pageLoaded)
                << "of"         << total
                << "results";

        if (pageLoaded == 0 || total == 0 || currentLoaded >= total) {
            qInfo() << "No more results";
            noResults = true;
            return;
        }

    });
}



        //qInfo() << "Response body:" << resp;
        // QFile file("resp.txt");
        // if (file.open(QIODevice::WriteOnly)) {
        //     file.write(resp);
        //     file.close();
        //     qInfo() << "Response saved to resp.txt";
        // } else {
        //     qWarning() << "Cannot open file for writing:" << file.errorString();
        // }