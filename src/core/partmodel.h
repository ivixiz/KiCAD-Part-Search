// FILE: core/partmodel.h
#pragma once
#include "partdata.h"
#include "api/partsupplier.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QAbstractListModel>

extern QNetworkAccessManager* globalNetMgr;
extern float VAT;
extern int currentLoaded;

class PartModel : public QAbstractListModel {
    Q_OBJECT
  public:
    enum Roles { PartRole = Qt::UserRole + 1 };

    explicit PartModel(PartSupplier* supplier, QObject* parent = nullptr);
    void addParts(const QList<PartData>& parts);
    int rowCount(const QModelIndex& = {}) const override;
    QVariant data(const QModelIndex& idx, int role) const override;
    QHash<int,QByteArray> roleNames() const override;
    void clear();

  private:
    QList<PartData> items;
    QNetworkAccessManager* netMgr;
    PartSupplier* supplier = nullptr;

    void fetchThumbnail(int row);
};