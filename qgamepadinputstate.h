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

#ifndef INPUTSTATE_H
#define INPUTSTATE_H

#include <QPoint>
#include <QPointF>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QSizeF>
#include <QObject>
#include "qtgamepadglobal.h"
#include "qgamepadmanager.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class Q_GAMEPAD_EXPORT QGamepadInputState : public QObject
{
    Q_OBJECT
public:
    enum Buttons {
        Gamepad_A = 0x130,
        Gamepad_B,
        Gamepad_C,
        Gamepad_X,
        Gamepad_Y,
        Gamepad_Z,
        Gamepad_TL1,
        Gamepad_TR1,
        Gamepad_TL2,
        Gamepad_TR2,
        Gamepad_Select,
        Gamepad_Start,
        Gamepad_Mode,
        Gamepad_ThumbL,
        Gamepad_ThumbR,
        Gamepad_Up1 = 0x255,
        Gamepad_Down1,
        Gamepad_Left1,
        Gamepad_Right1,
        Gamepad_Up2,
        Gamepad_Down2,
        Gamepad_Left2,
        Gamepad_Right2,
        Gamepad_Up3,
        Gamepad_Down3,
        Gamepad_Left3,
        Gamepad_Right3
    };

    enum Hats {
        Hat_X1 = 0x10,
        Hat_Y1,
        Hat_X2,
        Hat_Y2,
        Hat_X3,
        Hat_Y3
    };

    enum Axis {
        Axis_X1 = 0x00,
        Axis_Y1,
        Axis_Z1,
        Axis_X2,
        Axis_Y2,
        Axis_Z2
    };

    QGamepadInputState(QObject *parent = 0);

public slots:

    void processMousePressEvent(QMouseEvent *event);
    void processMouseReleaseEvent(QMouseEvent *event);
    void processMouseMoveEvent(QMouseEvent *event);
    void processKeyPressEvent(QKeyEvent *event);
    void processKeyReleaseEvent(QKeyEvent *event);
    void processGamepadEvent(QGamepadInfo *info, quint64 time, int type, int number, int value);

public:
    QPointF mousePos() { return m_mousePos; }

    bool queryKey(int key) { return m_keyStateMap.value(key, false); }
    Qt::MouseButtons mouseButtons() { return m_buttonState; }
    Qt::KeyboardModifiers keyboardModifiers() { return m_modifierState; }
    bool queryGamepadButton(Buttons button, int id = 0);
    qreal queryGamepadAxis(Axis axis, int id = 0);

    //Debug
    void printInputState();

signals:
    void gamepadButtonReleased(int button, int id);
    void gamepadButtonPressed(int button, int id);
    void stateUpdated();

private:

    struct GamepadState {
        QGamepadInfo *info;
        QMap<Buttons, bool> buttonStateMap;
        QMap<Axis, int> axisStateMap;
    };

    void addGamepadButtonState(GamepadState *gamepadState, Buttons button, int value);
    void addGamepadAxisState(GamepadState *gamepadState, Axis axis, int value);

    QPointF m_mousePos;
    QMap<int, bool> m_keyStateMap;
    QMap<int,GamepadState*> m_gamepadStates;
    Qt::MouseButtons m_buttonState;
    Qt::KeyboardModifiers m_modifierState;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // INPUTSTATE_H
