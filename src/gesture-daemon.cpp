/*
 * (C) 2016 Kimmo Lindholm <kimmo.lindholm@eke.fi>
 * (C) 2019 Jami Kettunen <jami.kettunen@protonmail.com>
 *
 * Gesture daemon
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <QtCore/QCoreApplication>
#include "gesture-actions.h"
#include "gesture-enabler.h"

// D-Bus definitions to listen for gesture signals from MCE
#define MCE_SERVICE                 "com.nokia.mce"
#define MCE_SIGNAL_PATH             "/com/nokia/mce/signal"
#define MCE_SIGNAL_IF               "com.nokia.mce.signal"
#define POWER_BUTTON_TRIGGER_SIG    "power_button_trigger"


//
//  Daemon init
//

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    setlinebuf(stdout);
    setlinebuf(stderr);

    qDebug() << "Starting gesture-daemon version" << APPVERSION;

    Gestures gestures;
    GestureEnabler enabledGestures;

    static QDBusConnection systembusConnection = QDBusConnection::systemBus();
    systembusConnection.connect(MCE_SERVICE,
                                MCE_SIGNAL_PATH,
                                MCE_SIGNAL_IF,
                                POWER_BUTTON_TRIGGER_SIG,
                                &gestures, SLOT(handleGestureEvent(const QDBusMessage&)));

    return app.exec();
}
