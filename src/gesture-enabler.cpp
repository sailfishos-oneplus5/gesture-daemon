/*
 * (C) 2016 Kimmo Lindholm <kimmo.lindholm@eke.fi>
 * (C) 2019 Jami Kettunen <jami.kettunen@protonmail.com>
 *
 * Gesture daemon
 *
 */

#include "gesture-enabler.h"

//
//  Init
//

GestureEnabler::GestureEnabler(QObject *parent) : QObject(parent)
{
    dconfGestures = new MGConfItem("/apps/onyxgestures/enabled-gestures"); // e.g. ['double_tap', 'flashlight', 'music', 'camera', 'voicecall']

    /*
     *     UpVee_gesture = (buf[0] & BIT0)?1:0; //"V" -- flashlight
     *     DouSwip_gesture = (buf[0] & BIT1)?1:0;//"||" -- music
     *     DownVee_gesture = (buf[0] & BIT2)?1:0;//"^" -- voicecall
     *     LeftVee_gesture = (buf[0] & BIT3)?1:0; //">" -- music
     *     RightVee_gesture = (buf[0] & BIT4)?1:0;//"<" -- music
     *     Circle_gesture = (buf[0] & BIT6)?1:0; //"O" -- camera
     *     DouTap_gesture = (buf[0] & BIT7)?1:0; //double tap
     */

    gestureMasks.insert("camera", 0x40);
    gestureMasks.insert("music",0x1A);
    gestureMasks.insert("flashlight", 0x01);
    gestureMasks.insert("double_tap", 0x80);
    gestureMasks.insert("voicecall", 0x04);

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
    QStringList eg = dconfGestures->value(defGestures).toStringList();
    unsigned char mask = 0;
    QMap<QString, char>::iterator i;

    qDebug() << "Currently enabled gestures:" << eg;

    for (i = gestureMasks.begin(); i != gestureMasks.end(); ++i)
    {
        if (eg.contains(i.key()))
            mask |= i.value();
    }

    QFile outputFile( "/proc/touchpanel/gesture_enable" );

    if (outputFile.open(QIODevice::WriteOnly))
    {
        QDataStream out(&outputFile);
        out << mask;
        outputFile.close();
    }
    else
        qWarning() << "Failed to write";
}
