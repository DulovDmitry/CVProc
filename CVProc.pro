QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport concurrent

CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#build_nr.commands = $$PWD/build_inc.bat
#build_nr.depends = FORCE
#QMAKE_EXTRA_TARGETS += build_nr
#PRE_TARGETDEPS += build_nr

BUILD = "$$cat(build.txt)"

VERSION = 0.1.0.$$BUILD
QMAKE_TARGET_COMPANY = "ORG335a"
QMAKE_TARGET_PRODUCT = CVProc


SOURCES += \
    convolutemultiplefilesdialog.cpp \
    convolutionsettingsdialog.cpp \
    convolutionstatusbardialog.cpp \
    cvcurvedata.cpp \
    generalsettingsdialog.cpp \
    gradientcolor.cpp \
    graphdata.cpp \
    main.cpp \
    mainwindow.cpp \
    qcustomplot.cpp \
    showdatadialog.cpp \
    smoothmultiplefilesdialog.cpp \
    smoothsettingsdialog.cpp

HEADERS += \
    convolutemultiplefilesdialog.h \
    convolutionsettingsdialog.h \
    convolutionstatusbardialog.h \
    cvcurvedata.h \
    generalsettingsdialog.h \
    gradientcolor.h \
    graphdata.h \
    mainwindow.h \
    qcustomplot.h \
    showdatadialog.h \
    smoothmultiplefilesdialog.h \
    smoothsettingsdialog.h

FORMS += \
    convolutemultiplefilesdialog.ui \
    convolutionsettingsdialog.ui \
    convolutionstatusbardialog.ui \
    generalsettingsdialog.ui \
    mainwindow.ui \
    showdatadialog.ui \
    smoothmultiplefilesdialog.ui \
    smoothsettingsdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    CVProc_resources.qrc
