/*
 * (C) 2016 Kimmo Lindholm <kimmo.lindholm@eke.fi>
 *
 * Gesture daemon
 *
 */

#include <stdio.h>
#include "gesture-actions.h"
#include <QFile>
#include <QTextStream>
#include <mlite5/MGConfItem>

Gestures::Gestures(QObject *parent) :
    QObject(parent)
{
    _mpris2Service = QString();

    QDBusConnection bus = QDBusConnection::sessionBus();

    _serviceWatcher = new QDBusServiceWatcher(this);
    _serviceWatcher->setConnection(bus);
    connect(_serviceWatcher, &QDBusServiceWatcher::serviceOwnerChanged, this, &Gestures::ownerChanged);

    cameraDevice = new MGConfItem("/apps/jolla-camera/cameraDevice");
    cameraCaptureMode = new MGConfItem("/apps/jolla-camera/captureMode");

    cameraNeedRestoreSettings = false;
    cameraDeviceValue = cameraDevice->value().toString();
    cameraCaptureModeValue = cameraCaptureMode->value().toString();

    QObject::connect(cameraDevice, SIGNAL(valueChanged()), this, SLOT(cameraValueChanged()));
    QObject::connect(cameraCaptureMode, SIGNAL(valueChanged()), this, SLOT(cameraValueChanged()));
}

Gestures::~Gestures()
{
}

void Gestures::handleGestureEvent(const QDBusMessage & msg)
{
    QList<QVariant> args = msg.arguments();
    QString gestureEvent = args.at(0).toString();

    qDebug() << "MCE signal power_button_trigger says" << gestureEvent;

    /* event type EV_MSC
     * event code MSC_GESTURE
     *
     * event value 5 --> event5 --> flashlight
     * event value 6 --> event6 --> camera
     * event value 7 --> event7 --> voicecall
     * event value 8 --> event8 --> play/pause
     * event value 9 --> event9 --> next
     * event value 10 -> event10 -> previous
     */

    if (gestureEvent.compare("event5") == 0)
    {
        toggleFlashlight();
    }
    else if (gestureEvent.compare("event6") == 0)
    {
        showCameraViewfinder();
    }
    else if (gestureEvent.compare("event7") == 0)
    {
        showVoicecallUi();
    }
    else if (gestureEvent.compare("event8") == 0)
    {
        sendMpris2("PlayPause");
    }
    else if (gestureEvent.compare("event9") == 0)
    {
        sendMpris2("Next");
    }
    else if (gestureEvent.compare("event10") == 0)
    {
        sendMpris2("Previous");
    }
}

void Gestures::toggleFlashlight()
{
    int brightness = 0;

    QFile brf("/sys/class/leds/led:flash_torch/brightness");
    if (!brf.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "Flashlight toggle failed.";
        return;
    }

    QTextStream in( &brf );
    brightness = in.readLine().toInt();

    brf.close();

    if (brightness == 0)
        brightness = 255;
    else
        brightness = 0;

    if (!brf.open(QIODevice::WriteOnly))
    {
        qWarning() << "Flashlight toggle failed.";
        return;
    }

    QTextStream out (&brf);
    out << QString("%1").arg(brightness);

    brf.close();

}

void Gestures::showCameraViewfinder()
{
    cameraDeviceValue = cameraDevice->value().toString();
    cameraCaptureModeValue = cameraCaptureMode->value().toString();

    if (cameraDeviceValue == "primary" || cameraCaptureModeValue == "video")
        cameraNeedRestoreSettings = true;

    QDBusConnection bus = QDBusConnection::sessionBus();
    QDBusMessage call = QDBusMessage::createMethodCall("com.jolla.camera", "/", "com.jolla.camera", "showFrontViewfinder");

    bus.call(call, QDBus::NoBlock, 1);
}

void Gestures::cameraValueChanged()
{
    if (cameraNeedRestoreSettings)
    {
        QThread::msleep(100);
        cameraNeedRestoreSettings = false;
        cameraDevice->set(cameraDeviceValue);
        cameraCaptureMode->set(cameraCaptureModeValue);
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
            qDebug() << "no mpris2 service available";
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
        qWarning() << "mpris2 " << _mpris2Service << " failed:" << error.message();
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
            qDebug() << "found mpris2 service:" << _mpris2Service;
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
        qDebug() << "mpris2 service" << _mpris2Service << "removed from bus";
        _mpris2Service = QString();
    }
}
