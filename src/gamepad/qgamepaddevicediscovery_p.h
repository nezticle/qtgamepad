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

#ifndef JOYSTICKDEVICEDISCOVERY_H
#define JOYSTICKDEVICEDISCOVERY_H

#include <QObject>

#include <libudev.h>

class QSocketNotifier;

QT_BEGIN_NAMESPACE

class QGamepadDeviceDiscovery : public QObject
{
    Q_OBJECT
public:

    static QGamepadDeviceDiscovery *create(QObject *parent);
    ~QGamepadDeviceDiscovery();

    QStringList scanConnectedDevices();
    
signals:
    void deviceDetected(const QString &deviceNode);
    void deviceRemoved(const QString &deviceNode);
    
private slots:
    void handleUDevNotification();

private:
    explicit QGamepadDeviceDiscovery(struct udev *udev, QObject *parent = 0);

    struct udev *m_udev;
    struct udev_monitor *m_udevMonitor;
    int m_udevMonitorFileDescriptor;
    QSocketNotifier *m_udevSocketNotifier;
};

QT_END_NAMESPACE

#endif // JOYSTICKDEVICEDISCOVERY_H
