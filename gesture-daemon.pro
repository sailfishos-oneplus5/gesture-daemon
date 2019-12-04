TARGET = gestured

CONFIG += link_pkgconfig
PKGCONFIG += mlite5

QT += dbus
QT -= gui

DEFINES += "APPVERSION=\\\"$${SPECVERSION}\\\""

HEADERS += \
    src/gesture-actions.h \
    src/gesture-enabler.h

SOURCES += \
    src/gesture-daemon.cpp \
    src/gesture-actions.cpp \
    src/gesture-enabler.cpp

OTHER_FILES += \
    rpm/$${TARGET}.spec \
    config/$${TARGET}.service

target.path = /usr/bin/

systemduser.files = config/$${TARGET}.service
systemduser.path = /usr/lib/systemd/user/

INSTALLS += target systemduser
