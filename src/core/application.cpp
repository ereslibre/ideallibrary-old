/*
 * This file is part of the Ideal Library
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

/*!
 * @mainpage Ideal Library
 *
 * The Ideal Library is a free, intuitive, easy-to-use and consistent library meant to easily
 * develop powerful applications and libraries.
 *
 * In general terms, its purpose is to make life easier to developers. Exposing a consistent and
 * intuitive API it will allow developers handle from filesystem to GUI, going through extensions
 * and modules (plugins).
 */

/*!
 * @page workingWithSignals Working with signals
 *
 * Signals are a powerful way of telling other components that something has just happened. They will
 * let you write dynamic code that will react to things that are happening at other place of your
 * code.
 *
 * @section simpleSignal Defining a very simple signal with no arguments
 *
 * @code
 * #include <core/object.h>
 *
 * class MyObject
 *     : public IdealCore::Object
 * {
 * public:
 *     MyObject(Object *parent);
 *
 *     IDEAL_SIGNAL(myFirstSignal);
 * };
 * @endcode
 *
 * Now, some details that you should take into account:
 *
 *     - Signals can only be defined inside Object subclasses. Now any instance of class MyObject
 *       will have a signal called myFirstSignal.
 *
 *     - Signals can only be connected to methods in Object subclasses. Always that we speak about
 *       connecting a signal to an objects' method. We will see later that we can connect a signal
 *       to a static method or function.
 *
 * Let's continue with the implementation. Every signal declaration needs its initialization:
 *
 * @code
 * MyObject::MyObject(Object *parent)
 *     : Object(parent)
 *     , IDEAL_SIGNAL_INIT(myFirstSignal)
 * {
 * }
 * @endcode
 *
 * Note the way we are initializing it. We have to always initialize signals on the object
 * constructors. On all object constructors. If you forget to initialize a signal, like this:
 *
 * @code
 * MyObject::MyObject(Object *parent)
 *     : Object(parent) // missing IDEAL_SIGNAL_INIT on this constructor
 * {
 * }
 * @endcode
 *
 * When we try to compile this, we will get an error message similar to this one:
 *
 * @code
 * example.cpp: In constructor 'MyObject(IdealCore::Object*)':
 * example.cpp:85: error: call to 'IdealCore::Signal<>::Signal' declared with attribute error: missing call to IDEAL_SIGNAL_INIT on your class constructor
 * @endcode
 *
 * So the compiler will warn you: "missing call to IDEAL_SIGNAL_INIT on your class constructor".
 *
 * @section complexSignal Defining a complex signal with several arguments
 *
 * @code
 * #include <core/object.h>
 * #include <core/list.h>
 *
 * class MyObject
 *     : public IdealCore::Object
 * {
 * public:
 *     MyObject(Object *parent);
 *
 *     IDEAL_SIGNAL(myComplexSignal, bool, iint32, Object*, List<Object*>);
 * };
 * @endcode
 *
 * As you may imagine, the initialization is not very different:
 *
 * @code
 * MyObject::MyObject(Object *parent)
 *     : Object(parent)
 *     , IDEAL_SIGNAL_INIT(myComplexSignal, bool, iint32, Object*, List<Object*>)
 * {
 * }
 * @endcode
 *
 * @section severalSignals Defining several signals in an object
 *
 * We have learnt until now that a signal is composed by its name, and the parameters it can take
 * to carry information with it. Obviously, we can make our object contain several signals, let's
 * look at it:
 *
 * @code
 * #include <core/object.h>
 * #include <core/list.h>
 *
 * class MyObject
 *     : public IdealCore::Object
 * {
 * public:
 *     MyObject(Object *parent);
 *
 *     IDEAL_SIGNAL(myFirstSignal);
 *     IDEAL_SIGNAL(myComplexSignal, bool, iint32, Object*, List<Object*>);
 * };
 * @endcode
 *
 * Being the initialization:
 *
 * @code
 * MyObject::MyObject(Object *parent)
 *     : Object(parent)
 *     , IDEAL_SIGNAL_INIT(myFirstSignal)
 *     , IDEAL_SIGNAL_INIT(myComplexSignal, bool, iint32, Object*, List<Object*>)
 * {
 * }
 * @endcode
 *
 * Right now we have a complete way of filling our objects with signals. That is pretty useless if
 * we cannot do anything with them. Let's connect code. Now, suppose we are working with the last
 * MyObject example that we have written. Let's write MyOtherObject.
 *
 * @code
 * #include <core/object.h>
 *
 * class MyOtherObject
 *     : public IdealCore::Object
 * {
 * public:
 *     MyOtherObject(Object *parent);
 *
 *     void doSomethingEasy();
 *     void doSomethingComplex(bool a, iint32 b, Object *c, const List<Object*> &d);
 *
 *     IDEAL_SIGNAL(myForwardedSignal);
 * };
 * @endcode
 *
 * As you can see, we have defined two regular methods, as well as another signal. We are going to
 * learn the basics here: how to connect a signal to a method, and how to forward a signal. Let's
 * have a look at the implementation:
 *
 * @code
 * #include <core/application.h>
 *
 * MyOtherObject::MyOtherObject(Object *parent)
 *     : Object(parent)
 *     , IDEAL_SIGNAL_INIT(myForwardedSignal)
 * {
 * }
 *
 * void MyOtherObject::doSomethingEasy()
 * {
 *     IDEAL_SDEBUG("We have done something easy");
 * }
 *
 * void MyOtherObject::doSomethingComplex(bool, iint32, Object*, const List<Object*>&)
 * {
 *     IDEAL_SDEBUG("We have done something complex");
 * }
 *
 * int main(int argc, char **argv)
 * {
 *     IdealCore::Application app(argc, argv);
 *
 *     MyObject *myObject = new MyObject(&app);
 *     MyOtherObject *myOtherObject = new MyOtherObject(&app);
 *
 *     IdealCore::Object::connect(myObject->myFirstSignal, myOtherObject, &MyOtherObject::doSomethingEasy);
 *     IdealCore::Object::connect(myObject->myComplexSignal, myOtherObject, &MyOtherObject::doSomethingComplex);
 *     IdealCore::Object::connect(myObject->myFirstSignal, myOtherObject->myForwardedSignal);
 *
 *     return 0;
 * }
 * @endcode
 *
 * As you can see, in the main application we have connected three times:
 *
 *     - Regular connections:
 *         - myFirstSignal at myObject with doSomethingEasy at myOtherObject.
 *         - myComplexSignal at myObject with doSomethingComplex at myOtherObject.
 *
 *     - Signal forward:
 *         - myFirstSignal at myObject with myForwardedSignal at myOtherObject.
 *
 * But yet nothing happens. We have to emit signals for things to happen after having connected
 * them to other parts of the code. Let's revisit MyObject class for adding a method that will do
 * something and later will emit myFirstSignal:
 *
 * @code
 * #include <core/object.h>
 * #include <core/list.h>
 *
 * class MyObject
 *     : public IdealCore::Object
 * {
 * public:
 *     MyObject(Object *parent);
 *
 *     void modifyStateAndNotify();
 *
 *     IDEAL_SIGNAL(myFirstSignal);
 *     IDEAL_SIGNAL(myComplexSignal, bool, iint32, Object*, List<Object*>);
 * };
 * @endcode
 *
 * Being the initialization:
 *
 * @code
 * MyObject::MyObject(Object *parent)
 *     : Object(parent)
 *     , IDEAL_SIGNAL_INIT(myFirstSignal)
 *     , IDEAL_SIGNAL_INIT(myComplexSignal, bool, iint32, Object*, List<Object*>)
 * {
 * }
 *
 * void MyObject::modifyStateAndNotify()
 * {
 *     IDEAL_SDEBUG("We have modified an attribute and are going to emit myFirstSignal");
 *     emit(myFirstSignal);
 * }
 * @endcode
 *
 * Now, from the main application code we could do:
 *
 * @code
 * int main(int argc, char **argv)
 * {
 *     IdealCore::Application app(argc, argv);
 *
 *     MyObject *myObject = new MyObject(&app);
 *     MyOtherObject *myOtherObject = new MyOtherObject(&app);
 *
 *     IdealCore::Object::connect(myObject->myFirstSignal, myOtherObject, &MyOtherObject::doSomethingEasy);
 *     IdealCore::Object::connect(myObject->myComplexSignal, myOtherObject, &MyOtherObject::doSomethingComplex);
 *     IdealCore::Object::connect(myObject->myFirstSignal, myOtherObject->myForwardedSignal);
 *
 *     myObject->modifyStateAndNotify();
 *
 *     return 0;
 * }
 * @endcode
 *
 * This will cause the next output on terminal:
 *
 * @code
 * We have done something easy
 * @endcode
 *
 * We have emitted myFirstSignal which has no arguments, so it contains no information. Emitting
 * a signal that contains parameters is not harder. Let's suppose we wanted to emit myComplexSignal:
 *
 * @code
 * void MyObject::modifyStateAndNotify()
 * {
 *     IDEAL_SDEBUG("We have modified an attribute and are going to emit myFirstSignal");
 *     emit(myComplexSignal, false, 6, parent(), List<Object*>());
 * }
 * @endcode
 *
 * @section multislots Multislots
 *
 * Multislots are handy when you have to decide something depending on who is the parent of the
 * signal that called to that method. Let's see an example of a MediaPlayer with MediaButtons:
 *
 * @code
 * #include <core/object.h>
 * #include <core/application.h>
 *
 * class MediaButton
 *     : public IdealCore::Object
 * {
 * public:
 *     MediaButton(Object *parent);
 *
 *     IDEAL_SIGNAL(clicked);
 * };
 *
 * MediaButton::MediaButton(Object *parent)
 *     : Object(parent)
 *     , IDEAL_SIGNAL_INIT(clicked)
 * {
 * }
 *
 * class MediaPlayer
 *     : public IdealCore::Object
 * {
 * public:
 *     MediaPlayer(Object *parent);
 *
 *     void simulatePlayPauseClick();
 *     void simulateStopClick();
 *     void simulateQuitClick();
 *
 *     void executeAction(Object *sender);
 *
 * private:
 *     MediaButton *m_playPause;
 *     MediaButton *m_stop;
 *     MediaButton *m_quit;
 * };
 *
 * MediaPlayer::MediaPlayer(Object *parent)
 *     : Object(parent)
 *     , m_playPause(new MediaButton(this))
 *     , m_stop(new MediaButton(this))
 *     , m_quit(new MediaButton(this))
 * {
 *     connectMulti(m_playPause->clicked, this, &MediaPlayer::executeAction);
 *     connectMulti(m_stop->clicked, this, &MediaPlayer::executeAction);
 *     connectMulti(m_quit->clicked, this, &MediaPlayer::executeAction);
 * }
 *
 * void MediaPlayer::simulatePlayPauseClick()
 * {
 *     emit(m_playPause->clicked);
 * }
 *
 * void MediaPlayer::simulateStopClick()
 * {
 *     emit(m_stop->clicked);
 * }
 *
 * void MediaPlayer::simulateQuitClick()
 * {
 *     emit(m_quit->clicked);
 * }
 *
 * void MediaPlayer::executeAction(Object *sender)
 * {
 *     if (sender == m_playPause) {
 *         IDEAL_SDEBUG("Play or pause was clicked");
 *     } else if (sender == m_stop ) {
 *         IDEAL_SDEBUG("Stop was clicked");
 *     } else {
 *         IDEAL_SDEBUG("Quit was clicked");
 *     }
 * }
 *
 * int main(int argc, char **argv)
 * {
 *     IdealCore::Application app(argc, argv);
 *
 *     MediaPlayer *mediaPlayer = new MediaPlayer(&app);
 *     mediaPlayer->simulatePlayPauseClick();
 *     mediaPlayer->simulateStopClick();
 *     mediaPlayer->simulateQuitClick();
 *
 *     return 0;
 * }
 * @endcode
 *
 * Getting the output on terminal:
 *
 * @code
 * Play or pause was clicked
 * Stop was clicked
 * Quit was clicked
 * @endcode
 *
 * To sum up this example, you can see that clicked signal has no parameters. But we connected it
 * to a method that was taking one parameter (an Object*), that is exactly who emitted the signal
 * that called that method.
 *
 * Basically, the important thing to learn from this example, is that if you connect a signal to
 * a multislot, it has to contain as a first parameter an Object*, and then, the parameters the
 * signal has. For example, if we want to connect a signal defined by:
 *
 * @code
 * IDEAL_SIGNAL(myComplexSignal, bool, iint32, Object*, List<Object*>);
 * @endcode
 *
 * to a multislot, it would have to look like this:
 *
 * @code
 * void Whatever::aMultiSlotExample(Object *sender, bool a, iint32 b, Object *c, const List<Object*> &d)
 * {
 * }
 * @endcode
 *
 * The connection would be exactly the same:
 *
 * @code
 * IdealCore::Object::connectMulti(object->myComplexSignal, whatever, &Whatever::aMultiSlotExample);
 * @endcode
 */

#include <algorithm>

#include "application.h"
#include "private/application_p.h"
#include "timer.h"
#include "private/timer_p.h"
#include "interfaces/protocol_handler.h"
#include "private/object_p.h"
#include "private/event_dispatcher_p.h"
#include "event.h"

namespace IdealCore {

Application::Private::Private(Application *q)
    : m_prefixSet(false)
    , m_sleepTime(-1)
    , m_defaultSleepTime(500)
    , m_nextTimeout(-1)
    , q(q)
{
}

Application::Private::~Private()
{
}

void Application::Private::processEvents()
{
    iint32 timeToSleep;
    if (m_sleepTime != -1) {
        checkTimers();
        timeToSleep = m_sleepTime;
    } else {
        ContextMutexLocker cml(m_runningTimerListMutex);
        if (!m_runningTimerList.empty()) {
            m_sleepTime = m_runningTimerList.front()->d->m_interval;
            timeToSleep = m_sleepTime;
        } else {
            timeToSleep = m_defaultSleepTime;
        }
    }
    Timer::wait(timeToSleep);
}

void Application::Private::processDelayedDeletions()
{
    List<Object*> markedForDeletion;
    {
        ContextMutexLocker cml(m_markedForDeletionMutex);
        markedForDeletion = m_markedForDeletion;
        m_markedForDeletion.clear();
    }
    List<Object*>::iterator it;
    for (it = markedForDeletion.begin(); it != markedForDeletion.end(); ++it) {
        delete *it;
    }
}

bool Application::Private::timerSort(const Timer *left, const Timer *right)
{
    return left->d->m_remaining < right->d->m_remaining;
}

void Application::Private::checkTimers()
{
    List<Timer*> expiredTimerList;
    {
        ContextMutexLocker cml(m_runningTimerListMutex);
        if (m_runningTimerList.empty()) {
            return;
        }
        Timer *const firstExpiredTimer = m_runningTimerList.front();
        {
            // We control here the case in which the timer that was going to expire
            // (the head of the queue) was removed before actually expiring. With this
            // check we won't fire an incorrect timer when it had already time remaining.
            const iint32 msDelta = firstExpiredTimer->d->m_remaining - m_sleepTime;
            if (msDelta) {
                std::vector<Timer*>::iterator it;
                for (it = m_runningTimerList.begin(); it != m_runningTimerList.end(); ++it) {
                    Timer *const currTimer = *it;
                    currTimer->d->m_remaining -= m_sleepTime;
                }
                m_sleepTime = std::min(msDelta, m_defaultSleepTime);
                return;
            }
        }
        m_nextTimeout = firstExpiredTimer->d->m_remaining;
        std::vector<Timer*>::iterator it = m_runningTimerList.begin();
        while (it != m_runningTimerList.end()) {
            Timer *const currTimer = *it;
            if (currTimer->d->m_remaining == firstExpiredTimer->d->m_remaining) {
                if (currTimer->d->m_timeoutType == Timer::SingleShot) {
                    currTimer->d->m_state = Timer::Stopped;
                    it = m_runningTimerList.erase(it);
                } else {
                    currTimer->d->m_remaining = currTimer->d->m_interval;
                    ++it;
                }
                expiredTimerList.push_back(currTimer);
            } else {
                break;
            }
        }
        if (!m_runningTimerList.empty()) {
            while (it != m_runningTimerList.end()) {
                Timer *const currTimer = *it;
                currTimer->d->m_remaining -= m_nextTimeout;
                ++it;
            }
            std::sort(m_runningTimerList.begin(), m_runningTimerList.end(), PrivateImpl::timerSort);
            Timer *const nextTimer = m_runningTimerList.front();
            m_sleepTime = nextTimer->d->m_remaining;
        } else {
            m_sleepTime = -1;
        }
    }
    List<Timer*>::iterator it;
    for (it = expiredTimerList.begin(); it != expiredTimerList.end(); ++it) {
        Timer *const currTimer = *it;
        EventDispatcher *eventDispatcher = new EventDispatcher;
        Event *event = new Event(currTimer, Event::Timeout);
        eventDispatcher->postEvent(event);
        eventDispatcher->exec();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Application::Application(iint32 argc, ichar **argv, const String &name)
    : IDEAL_SIGNAL_INIT(invalidOption)
    , IDEAL_SIGNAL_INIT(missingParameter)
    , d(new PrivateImpl(this))
{
    d->m_argc = argc;
    d->m_argv = argv;
    d->m_name = name.empty() ? argv[0] : name;
    static_cast<Object*>(this)->d->m_application = this;
}

Application::~Application()
{
    delete d;
}

String Application::prefix() const
{
    return String();
}

Locale Application::locale() const
{
    return d->m_locale;
}

iint32 Application::exec()
{
    IDEAL_FOREVER {
        d->processEvents();
        d->processDelayedDeletions();
        d->checkFileWatches();
        d->unloadUnneededDynamicLibraries();
    }
    return 0;
}

}
