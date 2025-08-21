
// partcardwidget.h
#pragma once
#include <QWidget>
#include <QScrollBar>
#include "partdata.h"
extern float VAT;


class PartCardWidget : public QWidget {
  Q_OBJECT
public:
  explicit PartCardWidget(const PartData& part, QWidget* parent = nullptr);
private:
  bool eventFilter(QObject* obj, QEvent* event);
};