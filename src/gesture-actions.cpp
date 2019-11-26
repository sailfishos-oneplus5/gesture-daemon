/*
 * (C) 2016 Kimmo Lindholm <kimmo.lindholm@eke.fi>
 * (C) 2019 Jami Kettunen <jami.kettunen@protonmail.com>
 *
 * Gesture daemon
 *
 */

#include <stdio.h>
#include "gesture-actions.h"


//
//  MPRIS2 init
//

Gestures::Gestures(QObject *parent) : QObject(parent)
{
    // Setup MPRIS2 service watcher
    _mpris2Service = QString();

    QDBusConnection bus = QDBusConnection::sessionBus();

    _serviceWatcher = new QDBusServiceWatcher(this);
    _serviceWatcher->setConnection(bus);
    connect(_serviceWatcher, &QDBusServiceWatcher::serviceOwnerChanged, this, &Gestures::ownerChanged);
}

Gestures::~Gestures() {  }

//
//  Handle gestures via MCE signal
//

void Gestures::handleGestureEvent(const QDBusMessage & msg)
{
    QList<QVariant> args = msg.arguments();
    QString gestureEvent = args.at(0).toString();

    // Simply ignore other non-gesture actions
    if (!gestureEvent.startsWith("event")) return;

    qDebug() << "MCE signal power_button_trigger says" << gestureEvent;

    /* event type EV_MSC
     * event code MSC_GESTURE
     *
     * event value 4  -> event4  -> wakeup (not handled here)
     * event value 5  -> event5  -> play/pause
     * event value 6  -> event6  -> voicecall
     * event value 7  -> event7  -> flashlight
     * event value 8  -> event8  -> previous
     * event value 9  -> event9  -> next
     * event value 10 -> event10 -> camera
     */

    if (gestureEvent.compare("event5") == 0)       // Double swipe '↓↓'
        sendMpris2("PlayPause");
    else if (gestureEvent.compare("event6") == 0)  // Arrow up 'Ʌ'
        showVoicecallUi();
    else if (gestureEvent.compare("event7") == 0)  // Arrow down 'V'
        toggleFlashlight();
    else if (gestureEvent.compare("event8") == 0)  // Arrow left '<'
        sendMpris2("Previous");
    else if (gestureEvent.compare("event9") == 0)  // Arrow right '>'
        sendMpris2("Next");
    else if (gestureEvent.compare("event10") == 0) // Circle 'O'
        showCameraViewfinder();
}

//
//  Flashlight toggle
//

void Gestures::toggleFlashlight()
{
    QDBusConnection bus = QDBusConnection::sessionBus();
    QDBusMessage call = QDBusMessage::createMethodCall("com.jolla.settings.system.flashlight",
                                                       "/com/jolla/settings/system/flashlight",
                                                       "com.jolla.settings.system.flashlight",
                                                       "toggleFlashlight");

    QDBusError error = bus.call(call);
    if (error.isValid())
        qWarning() << "Flashlight toggle failed:" << error.message();
}

//
//  Launch camera
//

void Gestures::showCameraViewfinder()
{
    QDBusConnection bus = QDBusConnection::sessionBus();
    QDBusMessage call = QDBusMessage::createMethodCall("com.jolla.camera",
                                                       "/",
                                                       "com.jolla.camera.ui",
                                                       "showViewfinder");

    QVariantList args;
    args << QString("");
    call.setArguments(args);

    bus.call(call, QDBus::NoBlock, 1);
}

//
//  Call history
//

void Gestures::showVoicecallUi()
{
    QDBusConnection bus = QDBusConnection::sessionBus();
    QDBusMessage call = QDBusMessage::createMethodCall("com.jolla.voicecall.ui",
                                                       "/org/maemo/m",
                                                       "com.nokia.telephony.callhistory",
                                                       "launch");

    QVariantList args;
    args << QString("");
    call.setArguments(args);

    QDBusError error = bus.call(call);
    if (error.isValid())
        qWarning() << "Call history UI launch failed:" << error.message();
}

//
//  Music controls (play/pause doesn't work! next/prev go forward/back by 2 tracks instead of just 1!)
//

void Gestures::sendMpris2(const QString &methodName)
{
    if (_mpris2Service.isEmpty())
    {
        if (!getMpris2Service())
        {
            qDebug() << "No MPRIS2 service available";
            return;
        }
    }

    QDBusConnection bus = QDBusConnection::sessionBus();
    QDBusMessage call = QDBusMessage::createMethodCall(_mpris2Service,
                                                       "/org/mpris/MediaPlayer2",
                                                       "org.mpris.MediaPlayer2.Player",
                                                       methodName);

    qDebug() << "Invoking" << methodName << "on" << _mpris2Service;

    QDBusError error = bus.call(call);
    if (error.isValid())
        qWarning() << "MPRIS2 " << _mpris2Service << " failed:" << error.message();
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
            qDebug() << "Found MPRIS2 service:" << _mpris2Service;
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
        qDebug() << "MPRIS2 service" << _mpris2Service << "removed from bus";
        _mpris2Service = QString();
    }
}
