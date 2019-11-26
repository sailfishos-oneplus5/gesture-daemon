/*
 * (C) 2016 Kimmo Lindholm <kimmo.lindholm@eke.fi>
 * (C) 2019 Jami Kettunen <jami.kettunen@protonmail.com>
 *
 * Gesture daemon
 *
 */

#ifndef GESTUREENABLER_H
#define GESTUREENABLER_H

#include <QDebug>
#include <QObject>
#include <QFile>
#include <QDataStream>
#include <mlite5/MGConfItem>

class GestureEnabler : public QObject
{
    Q_OBJECT
public:
    explicit GestureEnabler(QObject *parent = 0);

public slots:
    void handleEnabledGestureChanged();

private:
    MGConfItem *dconfGestures;
    QStringList defGestures = { "double_tap", "voicecall", "camera", "music", "flashlight" };
    QMap<QString, char> gestureMasks;
};

#endif // GESTUREENABLER_H
