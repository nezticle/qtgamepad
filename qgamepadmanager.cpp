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

#include "qgamepadmanager.h"

#include "qgamepadhandler.h"
#include "qgamepaddevicediscovery_p.h"

#include <QtCore/QStringList>

#include <QtCore/QDebug>

QT_BEGIN_NAMESPACE

QGamepadManager::QGamepadManager(QObject *parent) :
    QObject(parent)
{
    m_gamepadDeviceDiscovery = QGamepadDeviceDiscovery::create(this);
    if (m_gamepadDeviceDiscovery) {
        // scan and add already connected joysticks
        QStringList devices = m_gamepadDeviceDiscovery->scanConnectedDevices();
        foreach (QString device, devices) {
            addGamepad(device);
        }

        connect(m_gamepadDeviceDiscovery, SIGNAL(deviceDetected(QString)), this, SLOT(addGamepad(QString)));
        connect(m_gamepadDeviceDiscovery, SIGNAL(deviceRemoved(QString)), this, SLOT(removeGamepad(QString)));
    }
}

QGamepadManager::~QGamepadManager()
{
    qDeleteAll(m_gamepads);
}

void QGamepadManager::handleGamepadEvent(quint64 time, QGamepadHandler::GamepadEventType type, int number, int value)
{
    QGamepadHandler *sender = qobject_cast<QGamepadHandler*>(this->sender());
    emit gamepadEvent(m_gamepadInfos.value(sender), time, (int)type, number, value);
}

void QGamepadManager::addGamepad(const QString &deviceNode)
{

    QGamepadHandler *handler;
    handler = QGamepadHandler::create(deviceNode);
    if (handler) {
        connect(handler, SIGNAL(handleGamepadEvent(quint64, QGamepadHandler::GamepadEventType, int, int)), this, SLOT(handleGamepadEvent(quint64, QGamepadHandler::GamepadEventType, int, int)));
        m_gamepads.insert(deviceNode, handler);
        m_gamepadInfos.insert(handler, new QGamepadInfo(m_gamepadInfos.count(), handler));
    } else {
        qWarning("Failed to open gamepad");
    }
}

void QGamepadManager::removeGamepad(const QString &deviceNode)
{
    if (m_gamepads.contains(deviceNode)) {
        QGamepadHandler *handler = m_gamepads.value(deviceNode);
        m_gamepads.remove(deviceNode);
        delete m_gamepadInfos.value(handler);
        m_gamepadInfos.remove(handler);
        delete handler;
    }
}

QT_END_NAMESPACE
