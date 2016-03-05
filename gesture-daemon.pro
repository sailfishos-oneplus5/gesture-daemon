TARGET = gestured

QT += dbus
QT -= gui

DEFINES += "APPVERSION=\\\"$${SPECVERSION}\\\""

target.path = /usr/bin/

systemduser.files = config/$${TARGET}.service
systemduser.path = /etc/systemd/user/

INSTALLS += target systemduser

message($${DEFINES})

SOURCES += \
    src/gesture-daemon.cpp \
    src/gesture-actions.cpp

OTHER_FILES += \
    rpm/$${TARGET}.spec \
    config/$${TARGET}.service

HEADERS += \
    src/gesture-actions.h
