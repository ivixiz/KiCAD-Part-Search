// FILE: ui/mainwindow.h
#pragma once
#include "api/partsupplier.h"
#include "core/config.h"

#include <QWidget>
#include <QNetworkReply>
#include <QListWidget>
#include <QDebug>
#include <QScrollBar>
#include <QGridLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QFile>
#include <QTextStream>
#include <QEvent>
#include <QKeyEvent>
#include <QComboBox>
#include <QPointer>
#include <QTimer>
#include <QPair>    
#include "api/partsupplier.h"
#include "api/mousersupplier.h"
#include "api/digikeysupplier.h"
#include "core/partmodel.h"
#include <QCheckBox>

//#define SAVE_RESPONSE


class MainWindow : public QWidget {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;
    QNetworkAccessManager netMgr;
private slots:
    void onSearch();
    void exportData();
    void onScrollChanged(int value);
    void fetchPartsAsync(PartSupplier* supplier, const QString& keyword, bool prepend);
private:
    PartModel* partModel;
    QListView* resultsView;
    QPushButton* btnSettings;
    QScrollArea* scrollSettings;
    QNetworkReply*        currentReply = nullptr;
    QPushButton*          searchButton;
    QLineEdit*            input;
    int offset = 0;
    QString currentKeyword;
    bool noResults = false;
    int resultssize = 0;
    bool firstReq = true;
    bool settingsButtonChecked = false;
    bool cardCreateFinished = false;
    int total = 0; 
    bool prepend = false;
    bool apierror = false;
    bool show_tip = true;
    quint64 m_requestEpoch = 0;
    PartSupplier* currentSupplier;

    void selectSupplier(int which);
    void killRowWidget(int row);
    void killAllWidgets();
    void resetSearch();
    void saveSettings(const QString &filename);
    void loadSettings(const QString &filename);
    void initSettings();
    void updatePlaceholder();
    bool eventFilter(QObject* obj, QEvent* event) override;
    void initSupplier(QComboBox* combobox);
    void createCards(const QList<PartData>& parts, int startRow, QPointer<PartSupplier> supplier, quint64 epoch, int scrollToRow = -1); 
    QWidget* createPartCard(const PartData& part, QPointer<PartSupplier> supplier,QWidget* parent);
};