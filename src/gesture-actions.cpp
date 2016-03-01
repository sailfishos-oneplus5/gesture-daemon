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
    int fd;
    char buf[5];
    bool ret = false;
    int brightness = 0;

    fd = open("/sys/class/leds/led:flash_torch/brightness", O_RDONLY);
    if (fd < 0)
        goto EXIT;

    if (!(read(fd, buf, sizeof(buf)) > 0))
    {
        close(fd);
        goto EXIT;
    }

    close(fd);

    brightness = atoi(buf);

    if (brightness == 0)
        brightness = 255;
    else
        brightness = 0;

    fd = open("/sys/class/leds/led:flash_torch/brightness", O_WRONLY);
    if (fd < 0)
        goto EXIT;

    sprintf(buf, "%d", brightness);
    if (!(write(fd, buf, strlen(buf)) > 0))
    {
        close(fd);
        goto EXIT;
    }

    close(fd);

    ret = true;
EXIT:
    return ret;
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
