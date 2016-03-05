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
    _mpris2Service = QString();

    QDBusConnection bus = QDBusConnection::sessionBus();

    _serviceWatcher->setConnection(bus);

    connect(_serviceWatcher, SIGNAL(serviceOwnerChanged(QString,QString,QString)), this, SLOT(ownerChanged(QString,QString,QString)));
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

void Gestures::showCameraViewfinder()
{
    QDBusConnection bus = QDBusConnection::sessionBus();
    QDBusMessage call = QDBusMessage::createMethodCall("com.jolla.camera", "/", "com.jolla.camera.ui", "showViewfinder");

    QDBusError error = bus.call(call);

    if (error.isValid())
    {
        qWarning() << "camera showViewfinder failed:" << error.message();
    }
}

void Gestures::showVoicecallUi()
{
    QDBusConnection bus = QDBusConnection::sessionBus();
    QDBusMessage call = QDBusMessage::createMethodCall("com.jolla.voicecall.ui", "/org/maemo/m", "com.nokia.telephony.callhistory", "launch");

    QVariantList args;
    args << QString("hello");
    call.setArguments(args);

    QDBusError error = bus.call(call);

    if (error.isValid())
    {
        qWarning() << "voicecall ui launch failed:" << error.message();
    }
}

void Gestures::sendMpris2(const QString &methodName)
{
    if (_mpris2Service.isEmpty())
    {
        if (!getMpris2Service())
        {
            qWarning() << "no mpris2 service available";
            return;
        }
    }

    QDBusConnection bus = QDBusConnection::sessionBus();
    QDBusMessage call = QDBusMessage::createMethodCall(_mpris2Service,
                                                       "/org/mpris/MediaPlayer2",
                                                       "org.mpris.MediaPlayer2.Player",
                                                       methodName);

    QDBusError error = bus.call(call);

    if (error.isValid())
    {
        qWarning() << "mpris2 failed (" << _mpris2Service << ")" << error.message();
    }
}

bool Gestures::getMpris2Service()
{
    bool ret = false;

    QDBusConnection bus = QDBusConnection::sessionBus();
    QDBusConnectionInterface *bus_iface = bus.interface();

    const QStringList &services = bus_iface->registeredServiceNames();
    foreach (QString service, services)
    {
        if (service.startsWith("org.mpris.MediaPlayer2."))
        {
            _mpris2Service = service;
            ret = true;
            break;
        }
    }
    return ret;
}

void Gestures::ownerChanged(const QString &name, const QString &oldOwner, const QString &newOwner)
{
    Q_UNUSED(oldOwner);

    if (name == _mpris2Service && newOwner.isEmpty())
    {
        _mpris2Service = QString();
        qDebug() << "mpris2 service removed from bus";
    }
}
