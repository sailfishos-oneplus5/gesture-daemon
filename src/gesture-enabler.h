#ifndef GESTUREENABLER_H
#define GESTUREENABLER_H

#include <QObject>
#include <QDebug>
#include <QFile>
#include <mlite5/MGConfItem>

class GestureEnabler : public QObject
{
    Q_OBJECT
public:
    explicit GestureEnabler(QObject *parent = 0);

public slots:
    void handleEnabledChanged();

private:
    MGConfItem *enabledGestures;
    QMap<QString, char> gestureMasks;

};

#endif // GESTUREENABLER_H
