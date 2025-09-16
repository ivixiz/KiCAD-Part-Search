#FILE main.pro
QT       += core gui network concurrent widgets
INCLUDEPATH += core
CONFIG   += c++17
TEMPLATE += app
TARGET   = part_search
DESTDIR = ../

SOURCES += \
    main.cpp \
    core/utils.cpp \
    api/mousersupplier.cpp \
    api/digikeysupplier.cpp \
    ui/mainwindow.cpp \
    core/partmodel.cpp \
    core/config.cpp \

HEADERS += \
    core/utils.h \
    core/config.h \
    api/partsupplier.h \
    api/mousersupplier.h \
    api/digikeysupplier.h \
    ui/mainwindow.h \
    core/partdata.h \
    core/partmodel.h \
    core/pricebreak.h \


RESOURCES += \
    resources/icons/resources.qrc

INCLUDEPATH += .

LIBS    += -lcurl

QMAKE_CLEAN += \
    qrc_resources.cpp \
    core/*.o \
    *.o