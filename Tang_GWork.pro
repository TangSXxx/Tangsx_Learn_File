QT       += core gui serialport opengl sql charts network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++11

DESTDIR = $$PWD/bin #执行程序可执行目录
              $$PWD/libmodbus

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    classfile.cpp \
    getjson.cpp \
    main.cpp \
    mainui.cpp \
    mainwindow.cpp \
    popupnotification.cpp \
    qcustomplot.cpp \
    axbopenglwidget.cpp \
    car.cpp \
    taskwindow.cpp \
    car_tcp.cpp \
    communicate.cpp \
    car.cpp     \
    XData.cpp \
    XStateClient.cpp

HEADERS += \
    classfile.h \
    getjson.h \
    mainui.h \
    mainwindow.h \
    popupnotification.h \
    qcustomplot.h \
    axbopenglwidget.h \
    car.h \
    taskwindow.h \
    car_tcp.h   \
    communicate.h \
    car.h   \
    XData.h \
    XStateClient.h

FORMS += \
    mainui.ui \
    mainwindow.ui \
    popupnotification.ui \
    taskwindow.ui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

win32:LIBS += -lws2_32

DISTFILES += \
