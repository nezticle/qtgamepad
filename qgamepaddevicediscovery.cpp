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

#include "qgamepaddevicediscovery_p.h"

#include <QtCore/QSocketNotifier>
#include <QtCore/QStringList>
#include <QtCore/QDebug>

QT_BEGIN_NAMESPACE

QGamepadDeviceDiscovery::QGamepadDeviceDiscovery(struct udev *udev, QObject *parent)
    : QObject(parent)
    , m_udev(udev)
    , m_udevMonitor(0)
    , m_udevMonitorFileDescriptor(-1)
    , m_udevSocketNotifier(0)
{
    if (!m_udev)
        return;

    m_udevMonitor = udev_monitor_new_from_netlink(m_udev, "udev");

    if (!m_udevMonitor) {
        qWarning("Unable to create an Udev monitor. No devices can be detected.");
        return;
    }

    udev_monitor_filter_add_match_subsystem_devtype(m_udevMonitor, "input", 0);
    udev_monitor_enable_receiving(m_udevMonitor);
    m_udevMonitorFileDescriptor = udev_monitor_get_fd(m_udevMonitor);

    m_udevSocketNotifier = new QSocketNotifier(m_udevMonitorFileDescriptor, QSocketNotifier::Read, this);
    connect(m_udevSocketNotifier, SIGNAL(activated(int)), this, SLOT(handleUDevNotification()));
}

QGamepadDeviceDiscovery::~QGamepadDeviceDiscovery()
{
    if (m_udevMonitor)
        udev_monitor_unref(m_udevMonitor);

    if (m_udev)
        udev_unref(m_udev);
}

QStringList QGamepadDeviceDiscovery::scanConnectedDevices()
{
    QStringList devices;

    if (!m_udev)
        return devices;

    udev_enumerate *ue = udev_enumerate_new(m_udev);
    udev_enumerate_add_match_subsystem(ue, "input");
    udev_enumerate_add_match_property(ue, "ID_INPUT_JOYSTICK", "1");

    if (udev_enumerate_scan_devices(ue) != 0) {
        qWarning() << "UDeviceHelper scan connected devices for enumeration failed";
        return devices;
    }

    udev_list_entry *entry;
    udev_list_entry_foreach (entry, udev_enumerate_get_list_entry(ue)) {
        const char *syspath = udev_list_entry_get_name(entry);
        udev_device *udevice = udev_device_new_from_syspath(m_udev, syspath);
        QString candidate = QString::fromUtf8(udev_device_get_devnode(udevice));
        if (candidate.startsWith(QLatin1String("/dev/input/event")))
            devices << candidate;

        udev_device_unref(udevice);
    }
    udev_enumerate_unref(ue);

    return devices;
}

void QGamepadDeviceDiscovery::handleUDevNotification()
{
    if (!m_udevMonitor)
        return;

    struct udev_device *dev;
    QString devNode;

    dev = udev_monitor_receive_device(m_udevMonitor);
    if (!dev)
        goto cleanup;

    const char *action;
    action = udev_device_get_action(dev);
    if (!action)
        goto cleanup;

    const char *str;
    str = udev_device_get_devnode(dev);
    if (!str)
        goto cleanup;

    if (qstrcmp(action, "add") == 0)
        emit deviceDetected(devNode);

    if (qstrcmp(action, "remove") == 0)
        emit deviceRemoved(devNode);

cleanup:
    udev_device_unref(dev);
}


QGamepadDeviceDiscovery *QGamepadDeviceDiscovery::create(QObject *parent)
{
    QGamepadDeviceDiscovery *handle = 0;
    struct udev *udev;

    udev = udev_new();
    if (udev) {
        handle = new QGamepadDeviceDiscovery( udev, parent);
    } else {
        qWarning("Failed to get udev library context.");
    }

    return handle;
}

QT_END_NAMESPACE
