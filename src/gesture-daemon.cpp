/*
 * (C) 2016 Kimmo Lindholm <kimmo.lindholm@eke.fi>
 *
 * Gesture daemon
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include "gesture-actions.h"
#include "mce/dbus-names.h"
#include <QtCore/QCoreApplication>

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    setlinebuf(stdout);
    setlinebuf(stderr);

    printf("Starting gesture-daemon version %s\n", APPVERSION);

    Gestures gestures;

    static QDBusConnection systembusConnection = QDBusConnection::systemBus();
    systembusConnection.connect(MCE_SERVICE,
                                MCE_SIGNAL_PATH,
                                MCE_SIGNAL_IF,
                                MCE_GESTURE_EVENT_SIG,
                          &gestures, SLOT(handleGestureEvent(const QDBusMessage&)));

    return app.exec();
}
