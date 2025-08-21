//FILE main.h
#pragma once

#include "core/utils.h"
#include "api/partsupplier.h"
#include "api/mousersupplier.h"
#include "api/digikeysupplier.h"
#include "core/partmodel.h"
#include "ui/partdelegate.h"
#include "ui/mainwindow.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//######### QT ##########
#include <QApplication>
#include <QListView>
#include <QMetaType>
#include <QWidget>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QPixmap>
#include <QDebug>
#include <QUrl>
#include <QRegularExpression>
#include <QScrollBar>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QIcon>
#include <QSize>
#include <QFile>

#define ICON_SETTINGS_PATH ":/icons/settings.svg"

//#define LOCK_AWAIT_RESPONSE //lock Search button avoiding spamming requests to server

extern QNetworkAccessManager* globalNetMgr;
extern QNetworkReply*         currentReply;
extern QPushButton*           searchButton;
//extern QVBoxLayout*           resultsLayout;
extern float                  VAT;
extern int                    currentOffset;
extern int                    currentLoaded;
extern QString                currentKeyword;
extern QListWidget*           resultsList;

