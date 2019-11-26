TARGET = gestured

CONFIG += link_pkgconfig
PKGCONFIG += mlite5

QT += dbus
QT -= gui

DEFINES += "APPVERSION=\\\"$${SPECVERSION}\\\""

target.path = /usr/bin/

systemduser.files = config/$${TARGET}.service
systemduser.path = /lib/systemd/user/

INSTALLS += target systemduser

message($${DEFINES})

SOURCES += \
    src/gesture-daemon.cpp \
    src/gesture-actions.cpp \
    src/gesture-enabler.cpp

OTHER_FILES += \
    rpm/$${TARGET}.spec \
    config/$${TARGET}.service

HEADERS += \
    src/gesture-actions.h \
    src/gesture-enabler.h
