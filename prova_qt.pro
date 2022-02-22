QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    ScreenRecorder.cpp \
    mainRecorder.cpp \
    transparent.cpp

HEADERS += \
    mainwindow.h \
    ScreenRecorder.h \
    transparent.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

QMAKE_CXXFLAGS += -D__STDC_CONSTANT_MACROS

win32:LIBS += -lpthread -lole32 -loleaut32


unix|win32: LIBS += -L$$PWD/lib/ -llibavcodec.dll -llibavdevice.dll -llibavfilter.dll -llibavformat.dll -llibavutil.dll -llibswresample.dll -llibswscale.dll

INCLUDEPATH += $$PWD/lib
DEPENDPATH += $$PWD/lib

unix|win32: LIBS += -L$$PWD/lib/ -lswresample

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include
