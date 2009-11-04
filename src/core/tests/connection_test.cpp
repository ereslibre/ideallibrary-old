/*
 * This file is part of the Ideal libraries
 * Copyright (C) 2009 Rafael Fernández López <ereslibre@ereslibre.es>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "connection_test.h"

#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <core/object.h>
#include <core/application.h>

using namespace IdealCore;

static Application *s_app = 0;

CPPUNIT_TEST_SUITE_REGISTRATION(ConnectionTest);

class SignalSpy
    : public Object
{
public:
    SignalSpy(Object *parent)
        : Object(parent)
        , m_signalsReceived(0)
    {
    }

    void receiveSignal()
    {
        ++m_signalsReceived;
    }

    void reset()
    {
        m_signalsReceived = 0;
    }

    iint32 signalsReceived() const
    {
        return m_signalsReceived;
    }

private:
    iint32 m_signalsReceived;
};

class Emitter
    : public Object
{
public:
    Emitter(Object *parent)
        : Object(parent)
        , IDEAL_SIGNAL_INIT(signal)
    {
    }

    void emitSignal()
    {
        emit(signal);
    }

    IDEAL_SIGNAL(signal);
};

void ConnectionTest::setUp()
{
    signalSpy = new SignalSpy(s_app);
}

void ConnectionTest::tearDown()
{
    delete signalSpy;
}

void ConnectionTest::connectTest()
{
    Emitter *emitter = new Emitter(s_app);
    {
        signalSpy->reset();
        Object::connect(emitter->signal, signalSpy, &SignalSpy::receiveSignal);
        emitter->emitSignal();
        CPPUNIT_ASSERT_EQUAL(1, signalSpy->signalsReceived());
    }
    {
        signalSpy->reset();
        Object::disconnect(emitter->signal, signalSpy, &SignalSpy::receiveSignal);
        emitter->emitSignal();
        CPPUNIT_ASSERT_EQUAL(0, signalSpy->signalsReceived());
    }
    {
        signalSpy->reset();
        Object::connect(emitter->signal, signalSpy, &SignalSpy::receiveSignal);
        Object::connect(emitter->signal, signalSpy, &SignalSpy::receiveSignal);
        emitter->emitSignal();
        CPPUNIT_ASSERT_EQUAL(2, signalSpy->signalsReceived());
        signalSpy->reset();
        Object::disconnectSender(emitter);
        emitter->emitSignal();
        CPPUNIT_ASSERT_EQUAL(0, signalSpy->signalsReceived());
    }
    {
        signalSpy->reset();
        Object::connect(emitter->signal, signalSpy, &SignalSpy::receiveSignal);
        Object::disconnectReceiver(signalSpy);
        emitter->emitSignal();
        CPPUNIT_ASSERT_EQUAL(0, signalSpy->signalsReceived());
    }
    {
        signalSpy->reset();
        Object::connect(emitter->signal, signalSpy, &SignalSpy::receiveSignal);
        emitter->setEmitBlocked(true);
        emitter->emitSignal();
        emitter->setEmitBlocked(false);
        Object::disconnect(emitter->signal, signalSpy, &SignalSpy::receiveSignal);
        CPPUNIT_ASSERT_EQUAL(0, signalSpy->signalsReceived());
    }
    {
        signalSpy->reset();
        Object::connect(emitter->signal, signalSpy, &SignalSpy::receiveSignal);
        emitter->setEmitBlocked(true);
        Object::emit(emitter->signal);
        emitter->setEmitBlocked(false);
        Object::disconnect(emitter->signal, signalSpy, &SignalSpy::receiveSignal);
        CPPUNIT_ASSERT_EQUAL(0, signalSpy->signalsReceived());
    }
    Emitter *secondEmitter = new Emitter(s_app);
    {
        signalSpy->reset();
        Object::connect(emitter->signal, secondEmitter->signal);
        Object::connect(secondEmitter->signal, signalSpy, &SignalSpy::receiveSignal);
        emitter->emitSignal();
        CPPUNIT_ASSERT_EQUAL(1, signalSpy->signalsReceived());
    }
}

int main(int argc, char **argv)
{
    Application app(argc, argv);
    s_app = &app;

    CppUnit::Test *suite = CppUnit::TestFactoryRegistry::getRegistry().makeTest();

    CppUnit::TextUi::TestRunner runner;
    runner.addTest(suite);

    runner.setOutputter(new CppUnit::CompilerOutputter(&runner.result(), std::cerr));
    bool wasSuccessful = runner.run();

    return wasSuccessful ? 0 : 1;
}
