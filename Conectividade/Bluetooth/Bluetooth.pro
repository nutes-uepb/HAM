#------------------------------------------------------------------------------------------------
#
# Copyright (c) 2016 Núcleo de Tecnologias Estratégicas em Saúde (NUTES)/Signove Tecnologia S/A .
# Licensed under the MIT license;
# You may obtain a copy of the License at:
# http://opensource.org/licenses/mit-license.php or see LICENSE file
#
# Project created by QtCreator 2016-01-04T18:36:56
#
#------------------------------------------------------------------------------------------------

QT       += core gui bluetooth

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Bluetooth
TEMPLATE = app


SOURCES += main.cpp\
        bluetoothdevice.cpp \
    bluetoothDeviceAdaptor.cpp

HEADERS  += bluetoothdevice.h \
    bluetoothDeviceAdaptor.h

QMAKE_CXXFLAGS += -std=gnu++0x
