/*
 * (C) 2016 Kimmo Lindholm <kimmo.lindholm@eke.fi>
 * (C) 2019 Jami Kettunen <jami.kettunen@protonmail.com>
 *
 * Gesture daemon
 *
 */

#ifndef GESTURE_ACTIONS_H
#define GESTURE_ACTIONS_H

#include <QObject>
#include <QtDBus/QtDBus>

class Gestures : public QObject
{
    Q_OBJECT

public:
    explicit Gestures(QObject *parent = 0);
    virtual ~Gestures();

public slots:
    void handleGestureEvent(const QDBusMessage&);

private slots:
    void ownerChanged(const QString &name, const QString &oldOwner, const QString &newOwner);

private:
    void toggleFlashlight();
    void sendMpris2(const QString &methodName);
    void showCameraViewfinder();
    void showVoicecallUi();
    bool getMpris2Service();

    QString _mpris2Service;
    QDBusServiceWatcher *_serviceWatcher;
};

#endif // GESTURE_ACTIONS_H
