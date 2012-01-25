INCLUDEPATH +=  ../interfaces \
                ../common/ 


DEPENDPATH += ../common/ 

TEMPLATE = lib
TARGET = chess
SOURCES = *.cpp \
          ./parser/*.cpp \
          ./parser/sexp/*.cpp \
          ./visclient/*.cpp 

HEADERS +=  *.h \
            ./parser/*.h \
            ./parser/sexp/*.h \
            ./visclient/*.h

CONFIG += debug plugin dll
debug:DEFINES += __DEBUG__
#QMAKE_CFLAGS_DEBUG += -pg
#QMAKE_CXXFLAGS_DEBUG += -pg
QMAKE_LFLAGS_DEBUG += -shared
QMAKE_LFLAGS_RELEASE += -shared
DEFINES += YY_NO_UNISTD_H
DESTDIR = ../plugins/

QT += opengl
