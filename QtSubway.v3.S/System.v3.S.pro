QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
RC_ICONS += app.ico

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# UID 329101171
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    my.cpp \
    rdialog.cpp \
    rldialog.cpp \
    sdialog.cpp \
    sldialog.cpp \
    swindow.cpp

HEADERS += \
    my.h \
    rdialog.h \
    rldialog.h \
    sdialog.h \
    sldialog.h \
    swindow.h

FORMS += \
    rdialog.ui \
    rldialog.ui \
    sdialog.ui \
    sldialog.ui \
    swindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DESTDIR=$$PWD/bin

RESOURCES += \
    rc.qrc
