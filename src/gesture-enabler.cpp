/*
 * (C) 2016 Kimmo Lindholm <kimmo.lindholm@eke.fi>
 * (C) 2019 Jami Kettunen <jami.kettunen@protonmail.com>
 *
 * Gesture daemon
 *
 */

#include <QFile>
#include <QTextStream>
#include "gesture-enabler.h"


//
//  Init
//

GestureEnabler::GestureEnabler(QObject *parent) : QObject(parent)
{
    dconfGestures = new MGConfItem("/apps/onyxgestures/enabled-gestures"); // e.g. ['double_tap', 'flashlight', 'music', 'camera', 'voicecall']

    // Restore previously enabled gestures on daemon startup
    handleEnabledGestureChanged();

    // Start listening for future gesture & setting enablement changes
    connect(dconfGestures, SIGNAL(valueChanged()), this, SLOT(handleEnabledGestureChanged()));
}

//
//  Gesture enabled toggle detection
//

void GestureEnabler::handleEnabledGestureChanged()
{
    QStringList enabledGestures = dconfGestures->value(defGestures).toStringList();

    qDebug() << "Currently enabled gestures:" << enabledGestures;

    QMap<QStringList, QString> newGestureStates;

    // Build a list of which gestures to enable/disable in sysfs
    for (const auto& gesture : defGestures)
    {
        QStringList items;
        if (gesture.compare("voicecall") == 0)
            items << "up_arrow_enable";
        else if (gesture.compare("flashlight") == 0)
            items << "down_arrow_enable";
        else if (gesture.compare("camera") == 0)
            items << "letter_o_enable";
        else if (gesture.compare("music") == 0)
            items << "double_swipe_enable" << "left_arrow_enable" << "right_arrow_enable";
        else
            items << gesture + "_enable";

        bool state = enabledGestures.contains(gesture);

        newGestureStates.insert(items,  (state ? "1" : "0"));
    }

    // Enable/disable all gestures as approperiate
    for(auto gestures : newGestureStates.keys())
    {
        // Value to write
        QString enabled_value = newGestureStates.value(gestures);

        // Go thru all grouped gestures
        for (const auto& gesture : gestures)
            writeGestureState(gesture, enabled_value);
    }
}

//
//  Enable / disable a specific gesture
//

void GestureEnabler::writeGestureState(QString gesture_enable, QString enabled_value)
{
    QString filePath = "/proc/touchpanel/" + gesture_enable; // e.g. "/proc/touchpanel/double_tap_enable"
    QFile outFile(filePath);
    if (outFile.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&outFile);
        stream << enabled_value; // "1" / "0"
    }
    else
        qWarning() << ("Failed to write '" + enabled_value + "' to " + filePath + "...");
}
