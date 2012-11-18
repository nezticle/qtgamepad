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

#include "gamepadmonitor.h"

GamepadMonitor::GamepadMonitor(QObject *parent)
    : QObject(parent)
    , m_manager(0)
    , m_inputState(0)
{
    m_manager = new QGamepadManager(this);
    m_inputState = new QGamepadInputState(this);

    connect(m_manager, SIGNAL(gamepadEvent(QGamepadInfo*,quint64,int,int,int)),
            m_inputState, SLOT(processGamepadEvent(QGamepadInfo*,quint64,int,int,int)));

    connect(m_inputState, SIGNAL(stateUpdated()), this, SLOT(printStatus()));
}

GamepadMonitor::~GamepadMonitor()
{
}

void GamepadMonitor::printStatus()
{
    m_inputState->printInputState();
}
