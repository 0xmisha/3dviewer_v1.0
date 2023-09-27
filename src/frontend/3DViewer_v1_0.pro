QT       += core gui openglwidgets opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

include(QtGifImage/src/gifimage/qtgifimage.pri)

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    myopenglwidget.cpp \
    ../backend/viewer.c \
    ../backend/affine.c

HEADERS += \
    mainwindow.h \
    myopenglwidget.h \
    ../backend/viewer.h

FORMS += \
    mainwindow.ui

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
