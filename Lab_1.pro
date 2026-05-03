QT += core

CONFIG += console
CONFIG -= app_bundle
CONFIG += c++17

TARGET = Lab_1

SOURCES += \
    main.cpp \
    ConsoleInput.cpp \
    FileProcessor.cpp \
    CryptoManager.cpp

HEADERS += \
    ConsoleInput.h \
    FileProcessor.h \
    CryptoManager.h

OPENSSL_DIR = $$PWD/third_party/openssl

INCLUDEPATH += $$OPENSSL_DIR/include

LIBS += $$OPENSSL_DIR/lib/libcrypto.a
LIBS += $$OPENSSL_DIR/lib/libz.a
LIBS += -lws2_32 -lgdi32 -lcrypt32