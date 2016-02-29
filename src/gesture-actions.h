/*
 * (C) 2016 Kimmo Lindholm <kimmo.lindholm@eke.fi>
 *
 * Gesture daemon
 *
 */

#ifndef GESTURE_ACTIONS_H
#define GESTURE_ACTIONS_H

#include <QObject>
#include <QtDBus/QtDBus>
#include <QTimer>
#include <QThread>

class Gestures : public QObject
{
    Q_OBJECT

public:
    explicit Gestures(QObject *parent = 0);
    virtual ~Gestures();

public slots:
    void handleGestureEvent(const QDBusMessage&);

};

#endif // GESTURE_ACTIONS_H
