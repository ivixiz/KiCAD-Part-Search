// FILE: core/partmodel.cpp
#include "partmodel.h"


PartModel::PartModel(PartSupplier* supplier, QObject* parent)
    : QAbstractListModel(parent),
      netMgr(new QNetworkAccessManager(this)),
      supplier(supplier)
{}

int PartModel::rowCount(const QModelIndex&) const {
    return items.size();
}
QVariant PartModel::data(const QModelIndex& idx, int role) const {
    if (!idx.isValid()) return {};
    if (role == PartRole)
        return QVariant::fromValue(items[idx.row()]);
    // also support Qt::DecorationRole so delegate can pull the pixmap
    if (role == Qt::DecorationRole)
        return items[idx.row()].image;
    return {};
}
QHash<int, QByteArray> PartModel::roleNames() const {
    return {{PartRole, "part"}};
}
void PartModel::clear() {
    beginResetModel();
    items.clear();     // assuming you store your PartData list in m_parts
    endResetModel();
}
void PartModel::addParts(const QList<PartData>& parts) {
    const int first = items.size();
    const int last  = first + parts.size() - 1;
    beginInsertRows({}, first, last);
    items.append(parts);
    endInsertRows();
    // start each image download:
    int row = first;
    for (; row <= last; ++row)
        fetchThumbnail(row);
    currentLoaded = row;
}
void PartModel::fetchThumbnail(int row) {
    auto& pd = items[row];
    if (!supplier) return;

    supplier->fetchThumbnail(pd, [this, row](QPixmap pix) {
        items[row].image = pix;
        QModelIndex idx = index(row, 0);
        emit dataChanged(idx, idx, {PartRole});
    });
}