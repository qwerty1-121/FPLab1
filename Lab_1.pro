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