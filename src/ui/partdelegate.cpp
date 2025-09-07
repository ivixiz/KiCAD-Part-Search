// FILE: ui/partdelegate.cpp
#include "partdelegate.h"
PartDelegate::PartDelegate(PartSupplier* s, QObject* parent)
    : QStyledItemDelegate(parent), supplier(s)
{}
QWidget* PartDelegate::createEditor(QWidget *parent,
                                    const QStyleOptionViewItem &,
                                    const QModelIndex &index) const
{
    PartData part = index.data(Qt::DisplayRole).value<PartData>();
    return supplier->createPartCard(part, parent);
}
