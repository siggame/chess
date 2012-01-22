INCLUDEPATH += ../interfaces \
 ../common/ 

DEPENDPATH += ../common/ 

TEMPLATE = lib
TARGET = chess
SOURCES = *.cpp
HEADERS += *.h

CONFIG += debug plugin
debug:DEFINES += __DEBUG__

DESTDIR = ../plugins/

QT += opengl
