#-------------------------------------------------
#
# Project created by QtCreator 2016-02-16T21:10:09
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = Lightdrops
TEMPLATE = app

win32 {
    DEFINES += WIN32
    INCLUDEPATH += C:/dev/cfitsio_64
    INCLUDEPATH += C:/dev/libraw/libraw
    INCLUDEPATH += C:/dev/opencv/build/include
    INCLUDEPATH += C:\dev\ArrayFire\v3\include

    LIBS += -LC:\dev\cfitsio_64 -lcfitsio
    LIBS += -LC:\dev\libraw\x64\Debug -llibraw
    LIBS += -LC:\dev\opencv\build\x64\vc14\lib -lopencv_world310
    LIBS += -LC:/dev/ArrayFire/v3/lib -lafopencl
}

macx {

    INCLUDEPATH += /usr/local/include

# cfitsio
    LIBS += -L/usr/local/lib -lcfitsio

# libraw
    LIBS += -L/usr/local/lib -lraw

# opencv "world" (it has all modules)
    LIBS += -L/usr/local/lib -lopencv_world

# arrayfire (not used at the moment)
    #LIBS += -L/usr/local/lib -lafopencl

# Setup Qt so Clang works with C++11
    QMAKE_CXXFLAGS += -stdlib=libc++
    LIBS += -stdlib=libc++
    QMAKE_CXXFLAGS += -std=c++11
}

SOURCES += main.cpp\
        rmainwindow.cpp \
    ropenglwidget.cpp \
    MyFitsImage.cpp \
    RawImage.cpp \
    rmat.cpp \
    rlistimagemanager.cpp \
    imagemanager.cpp \
    rtreewidget.cpp \
    rprocessing.cpp \
    parallelcalibration.cpp \
    rlineedit.cpp \
    RFrame.cpp \
    qcustomplot/qcustomplot.cpp \
    rsubwindow.cpp \
    data.cpp \
    circle.cpp \
    utilities.cpp \
    rgraphicsscene.cpp

HEADERS  += winsockwrapper.h \
    rmainwindow.h \
    ropenglwidget.h \
    MyFitsImage.h \
    RawImage.h \
    rmat.h \
    rlistimagemanager.h \
    imagemanager.h \
    rtreewidget.h \
    rprocessing.h \
    parallelcalibration.h \
    rlineedit.h \
    RFrame.h \
    qcustomplot/qcustomplot.h \
    rsubwindow.h \
    utilities.h \
    rsubwindow.h \
    data.h \
    typedefs.h \
    circle.h \
    rgraphicsscene.h


FORMS    += rmainwindow.ui

RESOURCES += \
    ressources.qrc

DISTFILES += \
    shaders/fragment16uc1.frag \
    shaders/fragment8.frag
