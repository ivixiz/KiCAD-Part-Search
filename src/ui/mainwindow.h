// FILE: ui/mainwindow.h

#pragma once

#include "api/partsupplier.h"

#include <QWidget>
#include <QNetworkReply>
#include <QListWidget>
#include <QDebug>
#include <QScrollBar>

extern QNetworkAccessManager* globalNetMgr;
extern QListWidget*           resultsList;
extern int                    currentLoaded;
extern QNetworkReply*         currentReply;
extern bool                   noResults;
extern bool                   firstReq;

class MainWindow : public QWidget {
    Q_OBJECT
public:
    void fetchPartsAsync(PartSupplier* supplier, const QString& keyword, int offset, bool dir);
    
};