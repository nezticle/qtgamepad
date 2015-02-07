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

#ifndef JOYSTICKMANAGER_H
#define JOYSTICKMANAGER_H

#include <QObject>
#include <QHash>
#include "qtgamepadglobal.h"
#include "qgamepadhandler.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QGamepadDeviceDiscovery;

class Q_GAMEPAD_EXPORT QGamepadInfo
{
public:
    QGamepadInfo(int id, QGamepadHandler *handler)
        : m_id(id)
        , m_handler(handler)
    {}
    int id() { return m_id; }
    QList<int> axisAvailable() { return m_handler->axisAvailable(); }
    int getAxisMinimum(int axis) {
        QGamepadHandler::AxisInfo *axisInfo = m_handler->axisInfo(axis);
        if(axisInfo) {
            return axisInfo->minimum;
        }
        return 0;
    }

    int getAxisMaximum(int axis) {
        QGamepadHandler::AxisInfo *axisInfo = m_handler->axisInfo(axis);
        if(axisInfo) {
            return axisInfo->maximum;
        }
        return 0;
    }
    int getAxisDeadZoneCenter(int axis) {
        QGamepadHandler::AxisInfo *axisInfo = m_handler->axisInfo(axis);
        if(axisInfo) {
            return axisInfo->deadzoneCenter;
        }
        return 0;
    }
    int getAxisDeadZoneRadius(int axis) {
        QGamepadHandler::AxisInfo *axisInfo = m_handler->axisInfo(axis);
        if(axisInfo) {
            return axisInfo->deadzoneRadius;
        }
        return 0;
    }

private:
    int m_id;
    QGamepadHandler *m_handler;
};

class Q_GAMEPAD_EXPORT QGamepadManager : public QObject
{
    Q_OBJECT
public:
    explicit QGamepadManager(QObject *parent = 0);
    ~QGamepadManager();

signals:
    void gamepadEvent(QGamepadInfo* info, quint64 time, int type, int number, int value);

private slots:
    void handleGamepadEvent(quint64 time, QGamepadHandler::GamepadEventType type, int number, int value);
    void addGamepad(const QString &deviceNode = QString());
    void removeGamepad(const QString &deviceNode);
    
private:
    QHash<QString, QGamepadHandler*> m_gamepads;
    QHash<QGamepadHandler*, QGamepadInfo*> m_gamepadInfos;
    QGamepadDeviceDiscovery *m_gamepadDeviceDiscovery;
};

QT_END_NAMESPACE

QT_END_HEADER

Q_DECLARE_METATYPE(QGamepadInfo*)

#endif // JOYSTICKMANAGER_H
