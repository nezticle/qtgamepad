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

#include "qgamepadhandler.h"

#include <QtCore/QSocketNotifier>
#include <qplatformdefs.h>

#include <errno.h>

#include <linux/input.h>
#include <sys/time.h>

#include <QtCore/qdebug.h>
#define NBITS(x) ((((x)-1)/(sizeof(long) * 8))+1)

QGamepadHandler *QGamepadHandler::create(const QString &device)
{
    int fd;


    fd = QT_OPEN(device.toLocal8Bit().constData(), O_RDONLY | O_NONBLOCK);
    if (fd >= 0) {
        return new QGamepadHandler(device, fd);
    } else {
        qWarning("Cannot open gamepad input device '%s': %s", qPrintable(device), strerror(errno));
        return 0;
    }
}

QGamepadHandler::QGamepadHandler(const QString &device, int fd)
    : m_device(device)
    , m_fd(fd)
    , m_notify(0)
{
    //socket notifier for events on the gamepad device
    QSocketNotifier *notifier;
    notifier = new QSocketNotifier(m_fd, QSocketNotifier::Read, this);
    connect(notifier, SIGNAL(activated(int)), this, SLOT(readGamepadData()));

    getAxisInfo();
}

QGamepadHandler::~QGamepadHandler()
{
    if (m_fd >= 0)
        QT_CLOSE(m_fd);

    foreach(int key, m_axisInfo.keys())
    {
        delete m_axisInfo.value(key);
    }
}

QGamepadHandler::AxisInfo* QGamepadHandler::axisInfo(int axis)
{
    return m_axisInfo.value(axis, 0);
}

const QList<int> QGamepadHandler::axisAvailable()
{
    return m_axisInfo.keys();
}

void QGamepadHandler::sendGamepadEvent(quint64 time, GamepadEventType type, int code, int value)
{
    emit handleGamepadEvent(time, type, code, value);
}

void QGamepadHandler::getAxisInfo()
{
    unsigned long absbit[NBITS(ABS_MAX)] = { 0 };

    if (ioctl(m_fd, EVIOCGBIT(EV_ABS, sizeof(absbit)), absbit) < 0)
        return;

    for (int i = 0; i < ABS_MISC; ++i) {
        /* Skip hats */
        if (i == ABS_HAT0X) {
            i = ABS_HAT3Y;
            continue;
        }

        struct input_absinfo absinfo;

        if (ioctl(m_fd, EVIOCGABS(i), &absinfo) < 0)
            continue;

        AxisInfo *currentAxis = new AxisInfo;
        currentAxis->minimum = absinfo.minimum;
        currentAxis->maximum = absinfo.maximum;
        currentAxis->deadzoneCenter = absinfo.value;
        currentAxis->deadzoneRadius = absinfo.flat;

        if (currentAxis->minimum != currentAxis->maximum) {
//            qDebug("Joystick has absolute axis: %x\n", i);
//            qDebug("Values = { %d, %d, %d, %d, %d }\n",
//                   absinfo.value, absinfo.minimum, absinfo.maximum,
//                   absinfo.fuzz, absinfo.flat);
            m_axisInfo.insert(i, currentAxis);
        } else {
            delete currentAxis;
        }
    }
}

void QGamepadHandler::readGamepadData()
{
    struct input_event buffer[32];
    int n = 0;

    forever {
        int result = QT_READ(m_fd, buffer + n, sizeof(buffer) - n);

        if (result == 0) {
            qWarning("Got EOF from the input device.");
            return;
        } else if (result < 0) {
            if (errno != EINTR && errno != EAGAIN) {
                qWarning("Could not read from input device: %s", strerror(errno));
                return;
            }
        } else {
            n += result;
            if (n % sizeof(buffer[0]) == 0)
                break;
        }
    }

    n /= sizeof(buffer[0]);

    for (int i = 0; i < n; ++i) {
        struct input_event *data = &buffer[i];

        int code = data->code;
        quint64 time = data->time.tv_sec * 1000000 + data->time.tv_usec;

        switch (data->type) {

        case EV_KEY:
            if (code >= BTN_MISC) {
                //code -= BTN_MISC;

                //Send button event
                //qDebug() << "Button: " << code << " : " << (data->value ? "pressed" : "released");
                sendGamepadEvent(time, Button, code, data->value);
            }
            break;
        case EV_ABS:
            if (code >= ABS_MISC) {
                break;
            }

            switch (code) {
            case ABS_HAT0X:
            case ABS_HAT0Y:
            case ABS_HAT1X:
            case ABS_HAT1Y:
            case ABS_HAT2X:
            case ABS_HAT2Y:
            case ABS_HAT3X:
            case ABS_HAT3Y:
                //code -= ABS_HAT0X;
                //Handle hat
                //qDebug() << "Hat: " << code << " : " << data->value;
                sendGamepadEvent(time, Hat, code, data->value);
                break;
            default:
                //Handle Axis event
                //qDebug() << "Axis: " << code << " : " << data->value;
                sendGamepadEvent(time, Axis, code, data->value);
                break;
            }
            break;
        case EV_REL:
            switch (code) {
            case REL_X:
            case REL_Y:
                //code -= REL_X;
                //Handle Ball event
                //qDebug() << "Ball: " << code << " : " << data->value;
                sendGamepadEvent(time, Ball, code, data->value);
                break;
            default:
                break;
            }
            break;
        default:
            break;

        }
    }
}
