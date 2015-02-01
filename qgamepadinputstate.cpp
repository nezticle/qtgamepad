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

#include "qgamepadinputstate.h"

#include <QtCore/QDebug>

QT_BEGIN_NAMESPACE

QGamepadInputState::QGamepadInputState(QObject *parent)
    : QObject(parent)
{
}

void QGamepadInputState::processMousePressEvent(QMouseEvent *event)
{
    m_mousePos = event->windowPos();
    m_buttonState = event->buttons();
    m_modifierState = event->modifiers();
    emit stateUpdated();
}

void QGamepadInputState::processMouseReleaseEvent(QMouseEvent *event)
{
    m_mousePos = event->windowPos();
    m_buttonState = event->buttons();
    m_modifierState = event->modifiers();
    emit stateUpdated();
}

void QGamepadInputState::processMouseMoveEvent(QMouseEvent *event)
{
    m_mousePos = event->windowPos();
    m_buttonState = event->buttons();
    m_modifierState = event->modifiers();
    emit stateUpdated();
}

void QGamepadInputState::processKeyPressEvent(QKeyEvent *event)
{
    m_keyStateMap.insert(event->key(), true);
    emit stateUpdated();
}

void QGamepadInputState::processKeyReleaseEvent(QKeyEvent *event)
{
    m_keyStateMap.insert(event->key(), false);
    emit stateUpdated();
}

void QGamepadInputState::processGamepadEvent(QGamepadInfo *info, quint64 time, int type, int number, int value)
{
    Q_UNUSED(time)

    QGamepadInputState::GamepadState *gamepadState = m_gamepadStates.value(info->id(), 0);

    //If this even comes from a joystick we've not seen before
    //map a new JoystickState
    if(!gamepadState)
    {
        gamepadState = new QGamepadInputState::GamepadState;
        gamepadState->info = info;
        m_gamepadStates.insert(info->id(), gamepadState);
    }

    if (type == QGamepadHandler::Button) {
        addGamepadButtonState(gamepadState, (Buttons)number, value);
    } else if (type == QGamepadHandler::Hat) {
        switch(number) {
        case Hat_X1:
            if (value == 1)
                addGamepadButtonState(gamepadState, Gamepad_Right1, true);
            else if (value == -1)
                addGamepadButtonState(gamepadState, Gamepad_Left1, true);
            else {
                addGamepadButtonState(gamepadState, Gamepad_Right1, false);
                addGamepadButtonState(gamepadState, Gamepad_Left1, false);
            }
            break;
        case Hat_Y1:
            if (value == 1)
                addGamepadButtonState(gamepadState, Gamepad_Down1, true);
            else if (value == -1)
                addGamepadButtonState(gamepadState, Gamepad_Up1, true);
            else {
                addGamepadButtonState(gamepadState, Gamepad_Down1, false);
                addGamepadButtonState(gamepadState, Gamepad_Up1, false);
            }
            break;
        case Hat_X2:
            if (value == 1)
                addGamepadButtonState(gamepadState, Gamepad_Right2, true);
            else if (value == -1)
                addGamepadButtonState(gamepadState, Gamepad_Left2, true);
            else {
                addGamepadButtonState(gamepadState, Gamepad_Right2, false);
                addGamepadButtonState(gamepadState, Gamepad_Left2, false);
            }
            break;
        case Hat_Y2:
            if (value == 1)
                addGamepadButtonState(gamepadState, Gamepad_Down2, true);
            else if (value == -1)
                addGamepadButtonState(gamepadState, Gamepad_Up2, true);
            else {
                addGamepadButtonState(gamepadState, Gamepad_Down2, false);
                addGamepadButtonState(gamepadState, Gamepad_Up2, false);
            }
            break;
        case Hat_X3:
            if (value == 1)
                addGamepadButtonState(gamepadState, Gamepad_Right3, true);
            else if (value == -1)
                addGamepadButtonState(gamepadState, Gamepad_Left3, true);
            else {
                addGamepadButtonState(gamepadState, Gamepad_Right3, false);
                addGamepadButtonState(gamepadState, Gamepad_Left3, false);
            }
            break;
        case Hat_Y3:
            if (value == 1)
                addGamepadButtonState(gamepadState, Gamepad_Down3, true);
            else if (value == -1)
                addGamepadButtonState(gamepadState, Gamepad_Up3, true);
            else {
                addGamepadButtonState(gamepadState, Gamepad_Down3, false);
                addGamepadButtonState(gamepadState, Gamepad_Up3, false);
            }
            break;
        }
    } else if(type == QGamepadHandler::Axis) {
        addGamepadAxisState(gamepadState, (Axis)number, value);
    }
    emit stateUpdated();
}


bool QGamepadInputState::queryGamepadButton(QGamepadInputState::Buttons button, int id)
{
    GamepadState *currentState = m_gamepadStates.value(id, 0);

    if(currentState) {
        return currentState->buttonStateMap.value(button, false);
    }

    return false;
}

qreal QGamepadInputState::queryGamepadAxis(QGamepadInputState::Axis axis, int id)
{
    GamepadState *currentState = m_gamepadStates.value(id, 0);

    if(currentState) {
        //Normalize value returned by driver
        //Results will be from -1.0 -- 1.0
        // or 0.0 -- 1.0 depending on axis type

        int axisMinimum = currentState->info->getAxisMinimum((int)axis);
        int axisMaximum = currentState->info->getAxisMaximum((int)axis);
        int axisDeadZoneCenter = currentState->info->getAxisDeadZoneCenter((int)axis);
        int axisDeadZoneRadius = currentState->info->getAxisDeadZoneRadius((int)axis);
        qreal currentValue = currentState->axisStateMap.value(axis, 0);


        //Case 0.0 - 1.0 (when minimum value is 0) triggers/throttles
        if (axisMinimum == 0) {
            return currentValue / (axisMaximum - axisMinimum);
        } else {
        //Case -1.0 - 1.0 (with deadzone) joysticks
            int deadZonePositive = axisDeadZoneCenter + axisDeadZoneRadius;
            int deadZoneNegative = axisDeadZoneCenter - axisDeadZoneRadius;

            //If the current value is within the deadzone
            if ((currentValue < deadZonePositive) && (currentValue > deadZoneNegative))
                return 0.0;

            //If the current value is on the positive side of the deadzone
            if ((currentValue > deadZonePositive)) {
                return (currentValue - deadZonePositive) / (axisMaximum - deadZonePositive);
            }

            //If the current value is on the negative side of the deadzone
            if ((currentValue < deadZoneNegative)) {
                return -(currentValue + deadZoneNegative) / (axisMinimum + deadZoneNegative);
            }
        }
    }

    return 0;
}

void QGamepadInputState::printInputState()
{
    qDebug() << "Mouse Window Position: " << m_mousePos;
    qDebug() << "Mouse Buttons State: " << m_buttonState;
    qDebug() << "Modifiers State: " << m_modifierState;


    foreach (int key, m_keyStateMap.keys())
    {
        qDebug() << "key: " << (Qt::Key)key << (m_keyStateMap.value(key) ? " is pressed: " : " is not pressed");
    }

    foreach (GamepadState *state, m_gamepadStates.values())
    {
        qDebug() << "Joystick #" << state->info->id();

        foreach (Buttons button, state->buttonStateMap.keys())
        {
            qDebug() << "button: " << (int)button << (queryGamepadButton(button, state->info->id()) ? " is pressed" : " is not pressed");
        }

        foreach (Axis axis, state->axisStateMap.keys())
        {
            qDebug() << "axis: " << (int)axis << " " << queryGamepadAxis(axis, state->info->id());
        }
    }
}

void QGamepadInputState::addGamepadButtonState(GamepadState *gamepadState, QGamepadInputState::Buttons button, int value)
{
    if (value) {
        emit gamepadButtonPressed(button, gamepadState->info->id());
    } else {
        emit gamepadButtonReleased(button, gamepadState->info->id());
    }
    gamepadState->buttonStateMap.insert(button, value);
}

void QGamepadInputState::addGamepadAxisState(GamepadState *gamepadState, QGamepadInputState::Axis axis, int value)
{
    gamepadState->axisStateMap.insert(axis, value);
}

QT_END_NAMESPACE
