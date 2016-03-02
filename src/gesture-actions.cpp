/*
 * (C) 2016 Kimmo Lindholm <kimmo.lindholm@eke.fi>
 *
 * Gesture daemon
 *
 */

#include <stdio.h>
#include "gesture-actions.h"
#include "mce/dbus-names.h"
#include "mce/mode-names.h"
#include <QFile>
#include <QTextStream>

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

    QString gestureEvent = args.at(0).toString();

    if (gestureEvent.compare(MCE_GESTURE_EVENT_FLASHLIGHT) == 0)
    {
        if (!toggleFlashlight())
            printf("Flashlight toggle failed.");
    }
    else if (gestureEvent.compare(MCE_GESTURE_EVENT_CAMERA) == 0)
    {
        showCameraViewfinder();
    }
    else if (gestureEvent.compare(MCE_GESTURE_EVENT_VOICECALL) == 0)
    {
        showVoicecallUi();
    }
    else if (gestureEvent.compare(MCE_GESTURE_EVENT_MUSIC_PLAY_PAUSE) == 0)
    {
        sendMpris2("PlayPause");
    }
    else if (gestureEvent.compare(MCE_GESTURE_EVENT_MUSIC_NEXT_TRACK) == 0)
    {
        sendMpris2("Next");
    }
    else if (gestureEvent.compare(MCE_GESTURE_EVENT_MUSIC_PREV_TRACK) == 0)
    {
        sendMpris2("Previous");
    }
}

bool Gestures::toggleFlashlight()
{
    int brightness = 0;

    QFile brf("/sys/class/leds/led:flash_torch/brightness");
    if (!brf.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in( &brf );
    brightness = in.readLine().toInt();

    brf.close();

    if (brightness == 0)
        brightness = 255;
    else
        brightness = 0;

    if (!brf.open(QIODevice::WriteOnly))
        return false;

    QTextStream out (&brf);
    out << QString("%1").arg(brightness);

    brf.close();

    return true;
}

void Gestures::sendMpris2(const QString &methodName)
{
    Q_UNUSED(methodName)
}

void Gestures::showCameraViewfinder()
{

}

void Gestures::showVoicecallUi()
{

}

QString Gestures::getMpris2Interface()
{
    return QString("org.mpris.MediaPlayer2.jolla-mediaplayer");
}
