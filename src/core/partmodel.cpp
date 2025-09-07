//FILE: partmodel.cpp
#include "partmodel.h"

PartModel::PartModel(QObject *parent)
    : QAbstractListModel(parent)
{}
int PartModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return parts.size();
}
QVariant PartModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= parts.size()) return {};
    const PartData &part = parts[index.row()];
    //if      (role == Qt::DisplayRole) { return QVariant(); } 
    if (role == Qt::UserRole)    { return QVariant::fromValue(part); } 
    else if (role == Qt::SizeHintRole){ int h = SVAL(IMG_RESULT_SIZE_Y) + SVAL(CARD_MARGIN); return QSize(0, h); }
    return {};
}
void PartModel::addParts(const QList<PartData>& newParts) {
    if (newParts.isEmpty()) return;
    beginInsertRows(QModelIndex(), parts.size(), parts.size() + newParts.size() - 1);
    parts.append(newParts);
    endInsertRows();
}
void PartModel::prependParts(const QList<PartData>& newParts) {
    if (newParts.isEmpty()) return;
    beginInsertRows(QModelIndex(), 0, newParts.size() - 1);
    for (int i = newParts.size() - 1; i >= 0; --i) {
        parts.prepend(newParts[i]);
    }
    endInsertRows();
}
void PartModel::removeFirst() {
    if (parts.isEmpty()) return;
    beginRemoveRows(QModelIndex(), 0, 0);
    parts.removeFirst();
    endRemoveRows();
}
void PartModel::removeLast() {
    if (parts.isEmpty()) return;
    int last = parts.size() - 1;
    beginRemoveRows(QModelIndex(), last, last);
    parts.removeLast();
    endRemoveRows();
}
void PartModel::clear() {
    beginResetModel();
    parts.clear();
    endResetModel();
}
