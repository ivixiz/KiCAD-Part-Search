// FILE: ui/partdelegate.h
#pragma once

#include <QStyledItemDelegate>
#include <QWidget>
#include <QModelIndex>
#include <QStyleOptionViewItem>
#include "core/partdata.h"
#include "api/partsupplier.h"

class PartDelegate : public QStyledItemDelegate {
    Q_OBJECT
    PartSupplier* supplier;

public:
    explicit PartDelegate(PartSupplier* s, QObject* parent = nullptr);

    QWidget* createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;
};


// #include "core/partdata.h"
// #include "partcardwidget.h"
// #include <QStyledItemDelegate>
// #include <QPainter>
// #include <QStyleOptionViewItem>
// #include <QDesktopServices>
// #include <QHBoxLayout>
// #include <QScrollArea>
// #include <QLabel>
// #include <QVBoxLayout>
// #include <cfloat>

// extern const float VAT;

// class PartDelegate : public QStyledItemDelegate {
//     Q_OBJECT
// public:
//     using QStyledItemDelegate::QStyledItemDelegate;
//     QWidget* createEditor(QWidget* parent,const QStyleOptionViewItem& opt,const QModelIndex& idx) const override;
//     void setEditorData(QWidget* editor, const QModelIndex& idx) const override;
//     void updateEditorGeometry(QWidget* editor,const QStyleOptionViewItem& opt, const QModelIndex&) const override;
//     void paint(QPainter* painter, const QStyleOptionViewItem& opt, const QModelIndex& index) const override;
//     QSize sizeHint(const QStyleOptionViewItem& opt, const QModelIndex& index) const override;
// };
