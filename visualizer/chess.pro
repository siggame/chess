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

win32: {
CONFIG += static
} else {
LIBS += -lGLU
CONFIG += dll
}

CONFIG += release 
debug:DEFINES += __DEBUG__
QMAKE_LFLAGS_DEBUG += -shared -W
QMAKE_LFLAGS_RELEASE += -shared -W

DEFINES += YY_NO_UNISTD_H PERFT_FAST
DESTDIR = ../plugins/

QT += network opengl gui 
