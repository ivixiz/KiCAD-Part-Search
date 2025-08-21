#FILE main.pro
QT       += core gui network concurrent widgets
INCLUDEPATH += core
CONFIG   += c++17
TEMPLATE += app
TARGET   = HTMLscraper

SOURCES += \
    main.cpp \
    core/utils.cpp \
    api/partsupplier.cpp \
    api/mousersupplier.cpp \
    api/digikeysupplier.cpp \
    ui/mainwindow.cpp \
 #  core/partdata.cpp \
 #   core/partmodel.cpp \
 #   ui/partdelegate.cpp \
 #   partcardwidget.cpp \

HEADERS += \
    core/utils.h \
    api/partsupplier.h \
    api/mousersupplier.h \
    api/digikeysupplier.h \
    ui/mainwindow.h \
 #  core/partdata.h \
 #  core/partmodel.h \
 #  core/pricebreak.h \
 #   ui/partdelegate.h \
 #   partcardwidget.h \



RESOURCES += \
    resources/icons/resources.qrc

INCLUDEPATH += .

LIBS    += -lcurl

QMAKE_CLEAN += \
    qrc_resources.cpp \
    core/*.o \
    *.o