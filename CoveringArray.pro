TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    ca_solution.cpp \
    cmatrix.cpp

HEADERS += \
    ca_solution.h \
    cmatrix.h \
    utils.h

QMAKE_CXXFLAGS += -std=c++11
