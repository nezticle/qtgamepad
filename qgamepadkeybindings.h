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

#ifndef KEYMAP_H
#define KEYMAP_H

#include "qtgamepadglobal.h"
#include "qgamepadinputstate.h"

#include <QtCore/QMultiMap>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QGamepadInputState;

class Q_GAMEPAD_EXPORT QGamepadKeyBindings : public QObject
{
    Q_OBJECT

    enum Type {
        Key,
        Button,
        Axis,
        Mouse
    };

public:
    QGamepadKeyBindings(QGamepadInputState *inputState);

    void addAction(const QString &action, Qt::Key key, Qt::KeyboardModifiers modifiers = Qt::NoModifier);
    void addAction(const QString &action, Qt::MouseButton button, Qt::KeyboardModifiers modifiers = Qt::NoModifier);
    void addAction(const QString &action, QGamepadInputState::Buttons button, int controllerId = 0);
    void addAction(const QString &action, QGamepadInputState::Axis axis, int controllerId = 0);

    void registerMonitoredAction(const QString &action);
    void deregisterMonitoredAction(const QString &action);

    int checkAction(const QString &action);
    qreal checkAxisAction(const QString &action);

    void reset(); //Remove all keybindings

private slots:
    void checkMonitoredActions();

signals:
    void monitoredActionActivated(const QString &action);
    void monitoredActionDeactivated(const QString &action);

private:

    struct KeyBinding {
        QGamepadKeyBindings::Type type;
        int identifier;
        int modifiers;
        int controllerId;
    };

    QGamepadInputState *m_inputState;
    QMultiMap<QString, QGamepadKeyBindings::KeyBinding> m_bindingsMap;
    QMap<QString, bool> m_monitoredActions;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // KEYMAP_H
