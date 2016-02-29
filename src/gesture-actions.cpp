/*
 * (C) 2016 Kimmo Lindholm <kimmo.lindholm@eke.fi>
 *
 * Gesture daemon
 *
 */

#include <stdio.h>
#include "gesture-actions.h"
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

Gestures::Gestures(QObject *parent) :
    QObject(parent)
{
}

Gestures::~Gestures()
{
}

void Gestures::handleGestureEvent(const QDBusMessage & msg)
{
    QList<QVariant> args = msg.arguments();

    printf("MCE signal gesture_event_ind says: %s\n", qPrintable(args.at(0).toString()));
}
