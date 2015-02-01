/*
 * Copyright (c) 2012 Andy Nichols
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef JOYSTICKHANDLER_H
#define JOYSTICKHANDLER_H

#include <QObject>
#include <QMap>
#include "qtgamepadglobal.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QSocketNotifier;

class Q_GAMEPAD_EXPORT QGamepadHandler : public QObject
{
    Q_OBJECT
    Q_ENUMS(GamepadEventType)

public:
    struct AxisInfo {
        int minimum;
        int maximum;
        int deadzoneCenter;
        int deadzoneRadius;
    };

    enum GamepadEventType {
        Button,
        Axis,
        Hat,
        Ball
    };
    Q_DECLARE_FLAGS(GamepadEventTypes, GamepadEventType)

    static QGamepadHandler *create(const QString &device);
    ~QGamepadHandler();

    AxisInfo* axisInfo(int axis);
    const QList<int> axisAvailable();

signals:
    void handleGamepadEvent(quint64, QGamepadHandler::GamepadEventType, int, int);
    
private slots:
    void readGamepadData();

private:
    explicit QGamepadHandler(const QString &device, int fd);

    void sendGamepadEvent(quint64 time, GamepadEventType type, int code, int value);
    void getAxisInfo();

    QString m_device;
    int m_fd;
    QSocketNotifier *m_notify;
    QMap<int, AxisInfo*>  m_axisInfo;
};

QT_END_NAMESPACE

QT_END_HEADER

Q_DECLARE_OPERATORS_FOR_FLAGS(QGamepadHandler::GamepadEventTypes)
Q_DECLARE_METATYPE(QGamepadHandler*)

#endif // JOYSTICKHANDLER_H
