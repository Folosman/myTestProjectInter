QT       += core gui
INCLUDEPATH += C:\Users\nemti\Downloads\eigen-3.4.0
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    source/placeform.cpp \
    source/pointbycircle.cpp \
    source/circlebypoints.cpp \
    source/moveclass.cpp \
    source/pointcreator.cpp \
    source/commandclass.cpp \
    source/comment.cpp \
    source/main.cpp \
    source/mainwindow.cpp \
    source/pointonplace.cpp

HEADERS += \
    lib/commandclass.h \
    lib/comment.h \
    lib/mainwindow.h \
    lib/moveclass.h \
    lib/pointbycircle.h \
    lib/pointcreator.h \
    lib/circlebypoints.h \
    lib/pointonplace.h \
    lib/placeform.h

FORMS += \
    form/comment.ui \
    form/mainwindow.ui \
    form/pointcreator.ui \
    form/moveclass.ui \
    form/circlebypoints.ui \
    form/pointbycircle.ui \
    form/pointonplace.ui \
    form/placeform.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
