#-------------------------------------------------
#
# Project created by QtCreator 2014-09-24T14:37:20
#
#-------------------------------------------------

QMAKE_CXXFLAGS += -DCPU_Z80_USE_LOCAL_HEADER
QMAKE_CFLAGS   += -DCPU_Z80_USE_LOCAL_HEADER

QT += core opengl

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

LIBS += -lX11 -lasound

##QMAKE_CFLAGS += -march=bdver1 -O2 -pipe -fno-stack-protector
##QMAKE_CXXFLAGS += -march=bdver1 -O2 -pipe -fno-stack-protector

TARGET = μZX
TEMPLATE = app

P_SOURCES		   = ../../sources
P_RESOURCES		   = ../../resources
P_RESOURCES_COMMON	   = $$P_RESOURCES/common
P_RESOURCES_QT_UI	   = $$P_RESOURCES/Qt/UI

INCLUDEPATH += \
	/usr/include/C++ \
	$$P_SOURCES/common \
	$$P_SOURCES/common/emulators \
	$$P_SOURCES/Linux \
	$$P_SOURCES/Qt \

include($$P_SOURCES/common/common.pri)
include($$P_SOURCES/common/emulators/emulators.pri)
include($$P_SOURCES/Linux/Linux.pri)
include($$P_SOURCES/Qt/Qt.pri)
include($$P_RESOURCES/Qt/UI/UI.pri)
include($$P_RESOURCES/common/resources.pri)
