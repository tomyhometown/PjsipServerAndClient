TARGET = PjsipTestClient
TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11 -lstdc++ -o2 -fPIC
CONFIG += c++11

#CONFIG += link_pkgconfig
#PKGCONFIG += libpjproject

INCLUDEPATH += \
    $$PWD/../include

LIBS += \
    -L$$PWD/../lib \
    -lpjsua2-x86_64-unknown-linux-gnu \
    -lstdc++ \
    -lpjsua-x86_64-unknown-linux-gnu \
    -lpjsip-ua-x86_64-unknown-linux-gnu \
    -lpjsip-simple-x86_64-unknown-linux-gnu \
    -lpjsip-x86_64-unknown-linux-gnu \
    -lpjmedia-codec-x86_64-unknown-linux-gnu \
    -lpjmedia-x86_64-unknown-linux-gnu \
    -lpjmedia-videodev-x86_64-unknown-linux-gnu \
    -lpjmedia-audiodev-x86_64-unknown-linux-gnu \
    -lpjmedia-x86_64-unknown-linux-gnu \
    -lpjnath-x86_64-unknown-linux-gnu \
    -lpjlib-util-x86_64-unknown-linux-gnu \
    -lsrtp-x86_64-unknown-linux-gnu \
    -lresample-x86_64-unknown-linux-gnu \
    -lgsmcodec-x86_64-unknown-linux-gnu \
    -lspeex-x86_64-unknown-linux-gnu \
    -lilbccodec-x86_64-unknown-linux-gnu \
    -lg7221codec-x86_64-unknown-linux-gnu \
    -lyuv-x86_64-unknown-linux-gnu \
    -lwebrtc-x86_64-unknown-linux-gnu \
    -lpj-x86_64-unknown-linux-gnu \
    -lopenh264 \
    -lm -lrt -lpthread  -lasound -lssl -lcrypto


SOURCES += \
    pjsiptestclient.cpp \
    testclientaccount.cpp \
    testclientcall.cpp

HEADERS += \
    testclientaccount.h \
    testclientcall.h
