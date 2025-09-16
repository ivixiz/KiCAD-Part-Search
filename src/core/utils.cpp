//FILE utils.cpp
#include "core/utils.h"

QLabel* debugOutputLabel = nullptr;
void logHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    Q_UNUSED(context)
    QTextStream out(stdout);
    switch (type) {
    case QtDebugMsg:
        out << "[DEBUG] " << msg << "\n";
        break;
    case QtInfoMsg:
        out << "[INFO] " << msg << "\n";
        break;
    case QtWarningMsg:
        out << "[WARNING] " << msg << "\n";
        break;
    case QtCriticalMsg:
        out << "[CRITICAL] " << msg << "\n";
        break;
    case QtFatalMsg:
        out << "[FATAL] " << msg << "\n";
        abort();
    }
    out.flush();
    if (debugOutputLabel && type != QtDebugMsg) {
        debugOutputLabel->setText(msg);
    }
}// ############################################ FUNCTION END ################################################################
QString getStringField(const QJsonObject& obj, const QString& key) {
    if (!obj.contains(key))
        return {};
    QJsonValue v = obj.value(key);
    return (v.isString() && !v.toString().isEmpty())
           ? v.toString()
           : QString();
}// ############################################ FUNCTION END ################################################################
QString getField(const QJsonObject& obj, const QString& key, const QString& def) {
    QString s = getStringField(obj, key);
    return s.isEmpty() ? def : s;
}// ############################################ FUNCTION END ################################################################
QLabel* selectableLabel(const QString& html, bool rich) {
    auto lbl = new QLabel(html);
    if (rich) lbl->setTextFormat(Qt::RichText);
    lbl->setTextInteractionFlags(
        Qt::TextSelectableByMouse |
        (rich ? Qt::LinksAccessibleByMouse : Qt::NoTextInteraction));
    lbl->setWordWrap(true);
    return lbl;
}// ############################################ FUNCTION END ################################################################
QFrame* vPricesSeparator(int w, const QString& color) {
    auto sep = new QFrame;
    sep->setFrameShape(QFrame::VLine);
    sep->setFrameShadow(QFrame::Plain);
    sep->setStyleSheet(
        QString("min-width:%1px; background-color:%2;")
        .arg(w).arg(color));
    return sep;
}// ############################################ FUNCTION END ################################################################
