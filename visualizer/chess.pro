INCLUDEPATH +=  ../interfaces \
                ../common/ \
                ./parser/

DEPENDPATH += ../common/ 

TEMPLATE = lib
TARGET = chess
SOURCES = *.cpp \
          ./parser/*.cpp \
          ./visclient/*.cpp \
          ./frcperft/magicmoves.cpp \
          ./frcperft/MoveParser.cpp \
          ./frcperft/Bitboard.cpp \
          ./frcperft/Board.cpp \
          ./parser/sexp/*.cpp \
          ../common/*.cpp

HEADERS +=  *.h \
            ./parser/*.h \
            ./parser/sexp/*.h \
            ./visclient/*.h \
            ./frcperft/*.h 

CONFIG += debug plugin dll 
debug:DEFINES += __DEBUG__
#QMAKE_CFLAGS_DEBUG += -pg
#QMAKE_CXXFLAGS_DEBUG += -pg
QMAKE_LFLAGS_DEBUG += -shared -Wl 
QMAKE_LFLAGS_RELEASE += -shared -Wl 

win32:QMAKE_LFLAGS_DEBUG += -lws2_32
win32:QMAKE_LFLAGS_RELEASE += -lws2_32
win32:LIBS += /opt/mingw/mingw-cros-env/usr/i686-pc/mingw32/lib/ws2_32.a

DEFINES += YY_NO_UNISTD_H PERFT_FAST
DESTDIR = ../plugins/

QT += network opengl gui 
