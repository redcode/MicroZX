#-------------------------------------------------
#
# Project created by QtCreator 2014-09-24T14:37:20
#
#-------------------------------------------------

QMAKE_CXXFLAGS += -DEMULATION_CPU_Z80_NO_SLOTS
QMAKE_CFLAGS   += -DEMULATION_CPU_Z80_NO_SLOTS

QT += core guiQT opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

LIBS += -lX11

TARGET = μZX
TEMPLATE = app

P_SOURCES		   = ../../sources
P_SOURCES_COMMON	   = $$P_SOURCES/common
P_SOURCES_COMMON_EMULATORS = $$P_SOURCES/common/emulators
P_SOURCES_QT		   = $$P_SOURCES/Qt
P_RESOURCES		   = ../../resources
P_RESOURCES_COMMON	   = $$P_RESOURCES/common
P_RESOURCES_QT_UI	   = $$P_RESOURCES/Qt/UI

INCLUDEPATH += \
	$$P_SOURCES_COMMON \
	$$P_SOURCES_COMMON_EMULATORS \
	$$P_SOURCES_QT \

include(../../sources/common/common.pri)
include(../../sources/common/emulators/emulators.pri)
include(../../sources/Qt/Qt.pri)
include(../../resources/Qt/UI/UI.pri)
include(../../resources/common/resources.pri)
