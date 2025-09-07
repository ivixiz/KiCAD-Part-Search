// FILE: core/partmodel.h
#pragma once

#include <QAbstractListModel>
#include <QSize>
#include "config.h"
#include "partdata.h"

class PartModel : public QAbstractListModel {
    Q_OBJECT
public:
    explicit PartModel(QObject* parent = nullptr);

    QList<PartData> parts;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void addParts(const QList<PartData>& newParts);
    void prependParts(const QList<PartData>& newParts);
    void removeFirst();
    void removeLast();
    void clear();
};