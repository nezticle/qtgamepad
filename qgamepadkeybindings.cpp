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

#include "qgamepadkeybindings.h"

QT_BEGIN_NAMESPACE

QGamepadKeyBindings::QGamepadKeyBindings(QGamepadInputState *inputState)
    : QObject(inputState)
    , m_inputState(inputState)
{
    connect(m_inputState, SIGNAL(stateUpdated()), this, SLOT(checkMonitoredActions()));
}

void QGamepadKeyBindings::addAction(const QString &action, Qt::Key key, Qt::KeyboardModifiers modifiers)
{
    struct KeyBinding keyBinding;
    keyBinding.type = QGamepadKeyBindings::Key;
    keyBinding.identifier = key;
    keyBinding.modifiers = modifiers;
    keyBinding.controllerId = -1;

    m_bindingsMap.insert(action, keyBinding);
}

void QGamepadKeyBindings::addAction(const QString &action, Qt::MouseButton button, Qt::KeyboardModifiers modifiers)
{
    struct KeyBinding keyBinding;
    keyBinding.type = QGamepadKeyBindings::Mouse;
    keyBinding.identifier = button;
    keyBinding.modifiers = modifiers;
    keyBinding.controllerId = -1;

    m_bindingsMap.insert(action, keyBinding);
}

void QGamepadKeyBindings::addAction(const QString &action, QGamepadInputState::Buttons button, int controllerId)
{
    struct KeyBinding keyBinding;
    keyBinding.type = QGamepadKeyBindings::Button;
    keyBinding.identifier = button;
    keyBinding.modifiers = Qt::NoModifier;
    keyBinding.controllerId = controllerId;

    m_bindingsMap.insert(action, keyBinding);
}

void QGamepadKeyBindings::addAction(const QString &action, QGamepadInputState::Axis axis, int controllerId)
{
    struct KeyBinding keyBinding;
    keyBinding.type = QGamepadKeyBindings::Axis;
    keyBinding.identifier = axis;
    keyBinding.modifiers = Qt::NoModifier;
    keyBinding.controllerId = controllerId;

    m_bindingsMap.insert(action, keyBinding);
}

void QGamepadKeyBindings::registerMonitoredAction(const QString &action)
{
    if(!m_monitoredActions.contains(action))
        m_monitoredActions.insert(action, checkAction(action));
}

void QGamepadKeyBindings::deregisterMonitoredAction(const QString &action)
{
    m_monitoredActions.remove(action);
}

int QGamepadKeyBindings::checkAction(const QString &action)
{
    QList<KeyBinding> keyBindings = m_bindingsMap.values(action);

    foreach(KeyBinding binding, keyBindings) {
        switch(binding.type) {
        case QGamepadKeyBindings::Key:
            if (m_inputState->queryKey(binding.identifier)) {
                if (binding.modifiers == (binding.modifiers & m_inputState->keyboardModifiers()))
                    return 1;
            }
            break;
        case QGamepadKeyBindings::Button:
            if (m_inputState->queryGamepadButton((QGamepadInputState::Buttons)binding.identifier, binding.controllerId))
                return 1;
            break;
        case QGamepadKeyBindings::Mouse:
            if (binding.identifier == (binding.identifier & m_inputState->mouseButtons())) {
                if (binding.modifiers == (binding.modifiers & m_inputState->keyboardModifiers()))
                    return 1;
            }
            break;
        case QGamepadKeyBindings::Axis:
            return m_inputState->queryGamepadAxis((QGamepadInputState::Axis)binding.identifier, binding.controllerId);
            break;
        default:
            qWarning("Unknown action type");
            break;
        }
    }

    return 0;
}

qreal QGamepadKeyBindings::checkAxisAction(const QString &action)
{
    QList<KeyBinding> keyBindings = m_bindingsMap.values(action);

    foreach(KeyBinding binding, keyBindings) {
        switch(binding.type) {
        case QGamepadKeyBindings::Axis:
            return m_inputState->queryGamepadAxis((QGamepadInputState::Axis)binding.identifier, binding.controllerId);
            break;
        default:
            qWarning("Keymap::checkAxisAction used without Axis type");
            break;
        }
    }

    return 0.0;
}

void QGamepadKeyBindings::reset()
{
    m_bindingsMap.clear();
    m_monitoredActions.clear();
}

void QGamepadKeyBindings::checkMonitoredActions()
{
    foreach (QString action, m_monitoredActions.keys()) {
        int currentValue = checkAction(action);
        if (currentValue != m_monitoredActions.value(action, 0)) {
            if (currentValue)
                emit monitoredActionActivated(action);
            else
                emit monitoredActionDeactivated(action);
            //Reset stored value
            m_monitoredActions.insert(action, currentValue);
        }
    }
}

QT_END_NAMESPACE
