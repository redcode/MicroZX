#-------------------------------------------------
#
# Project created by QtCreator 2014-09-24T14:37:20
#
#-------------------------------------------------

QMAKE_CXXFLAGS += -DEMULATION_CPU_Z80_NO_SLOTS
QMAKE_CFLAGS   += -DEMULATION_CPU_Z80_NO_SLOTS

QT += core guiQT opengl multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

LIBS += -lX11 -lasound

TARGET = μZX
TEMPLATE = app

P_SOURCES		   = ../../sources
P_RESOURCES		   = ../../resources
P_RESOURCES_COMMON	   = $$P_RESOURCES/common
P_RESOURCES_QT_UI	   = $$P_RESOURCES/Qt/UI

INCLUDEPATH += \
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
