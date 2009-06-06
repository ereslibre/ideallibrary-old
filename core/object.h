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

#ifndef OBJECT_H
#define OBJECT_H

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
 * #include <core/ideal_list.h>
 *
 * class MyObject
 *     : public IdealCore::Object
 * {
 * public:
 *     MyObject(Object *parent);
 *
 *     IDEAL_SIGNAL(myComplexSignal, bool, int, Object*, List<Object*>);
 * };
 * @endcode
 *
 * As you may imagine, the initialization is not very different:
 *
 * @code
 * MyObject::MyObject(Object *parent)
 *     : Object(parent)
 *     , IDEAL_SIGNAL_INIT(myComplexSignal, bool, int, Object*, List<Object*>)
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
 * #include <core/ideal_list.h>
 *
 * class MyObject
 *     : public IdealCore::Object
 * {
 * public:
 *     MyObject(Object *parent);
 *
 *     IDEAL_SIGNAL(myFirstSignal);
 *     IDEAL_SIGNAL(myComplexSignal, bool, int, Object*, List<Object*>);
 * };
 * @endcode
 *
 * Being the initialization:
 *
 * @code
 * MyObject::MyObject(Object *parent)
 *     : Object(parent)
 *     , IDEAL_SIGNAL_INIT(myFirstSignal)
 *     , IDEAL_SIGNAL_INIT(myComplexSignal, bool, int, Object*, List<Object*>)
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
 *     void doSomethingComplex(bool a, int b, Object *c, const List<Object*> &d);
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
 * void MyOtherObject::doSomethingComplex(bool, int, Object*, const List<Object*>&)
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
 * #include <core/ideal_list.h>
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
 *     IDEAL_SIGNAL(myComplexSignal, bool, int, Object*, List<Object*>);
 * };
 * @endcode
 *
 * Being the initialization:
 *
 * @code
 * MyObject::MyObject(Object *parent)
 *     : Object(parent)
 *     , IDEAL_SIGNAL_INIT(myFirstSignal)
 *     , IDEAL_SIGNAL_INIT(myComplexSignal, bool, int, Object*, List<Object*>)
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
 *     switch (sender) {
 *         case m_playPause:
 *             IDEAL_SDEBUG("Play or pause was clicked");
 *             break;
 *         case m_stop:
 *             IDEAL_SDEBUG("Stop was clicked");
 *             break;
 *         default:
 *             IDEAL_SDEBUG("Quit was clicked");
 *             application()->quit();
 *             break;
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
 * To sum up this example, you can see that clicked signal has no parameters. But we connected it
 * to a method that was taking one parameter (an Object*), that is exactly who emitted the signal
 * that called that method.
 *
 * Basically, the important thing to learn from this example, is that if you connect a signal to
 * a multislot, it has to contain as a first parameter an Object*, and then, the parameters the
 * signal has. For example, if we want to connect a signal defined by:
 *
 * @code
 * IDEAL_SIGNAL(myComplexSignal, bool, int, Object*, List<Object*>);
 * @endcode
 *
 * to a multislot, it would have to look like this:
 *
 * @code
 * void Whatever::aMultiSlotExample(Object *sender, bool a, int b, Object *c, const List<Object*> &d)
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

#include <ideal_export.h>
#include <core/ideal_signal.h>
#include <core/signal_resource.h>

/**
  * The IdealCore namespace.
  *
  * It contains the basic pillars of the Ideal Library. All other modules are built upon this one.
  * If you want to use the Ideal Library, you will have to use IdealCore.
  */
namespace IdealCore {

class Application;

/**
  * @class Object object.h core/object.h
  *
  * The base class for Ideal library usage. Inheriting this class will allow you to use signals
  * and connect them to methods in any class that inherits IdealCore::Object. It contains the basic
  * functionality offered by the Ideal Library.
  *
  * You can check how to work with signals with several examples:
  *     - @ref workingWithSignals
  *
  * @author Rafael Fernández López <ereslibre@ereslibre.es>
  */
class IDEAL_EXPORT Object
    : public SignalResource
{
    // This friend classes are allowed to use the default constructor
    friend class Application;
    friend class Extension;
    friend class Module;
    friend class Timer;

public:
    Object(Object *parent);
    virtual ~Object();

    /**
      * Sets whether children of this object should be deleted recursively when this object is.
      */
    void setDeleteChildrenRecursively(bool deleteChildrenRecursively);

    /**
      * @return Whether this object children will be removed when this object is.
      *
      * @note true by default.
      */
    bool isDeleteChildrenRecursively() const;

    /**
      * Sets whether signals are blocked for this object or not. If @p blockedSignals is true, this
      * object won't receive any call coming from a signal.
      */
    void setBlockedSignals(bool blockedSignals);

    /**
      * @return Whether signals are blocked for this object or not.
      *
      * @note false by default.
      */
    bool areSignalsBlocked() const;

    /**
      * Sets whether this object can emit signals.
      *
      * @note destroyed signal will be emitted even if emit is blocked for that object.
      */
    void setEmitBlocked(bool emitBlocked);

    /**
      * @return Whether this object can emit signals.
      *
      * @note false by default.
      */
    bool isEmitBlocked() const;

    /**
      * @return The list of children of this object.
      */
    List<Object*> children() const;

    /**
      * @return The parent of this object.
      */
    Object *parent() const;

    /**
      * Reparents this object to @p parent.
      */
    void reparent(Object *parent);

    /**
      * @return The application this object belongs to.
      */
    Application *application() const;

    /**
      * Connects @p signal to the method @p member on @p receiver.
      *
      * See @ref workingWithSignals
      */
    template <typename Receiver, typename Member, typename... Param>
    static inline void connect(const Signal<Param...> &signal, Receiver *receiver, Member member)
    {
        const_cast<Signal<Param...>*>(&signal)->connect(receiver, member);
    }

    /**
      * Connects @p signal to the multi method @p member on @p receiver.
      *
      * See @ref workingWithSignals
      */
    template <typename Receiver, typename Member, typename... Param>
    static inline void connectMulti(const Signal<Param...> &signal, Receiver *receiver, Member member)
    {
        const_cast<Signal<Param...>*>(&signal)->connectMulti(receiver, member);
    }

    /**
      * Forwards @p signal to another signal @p receiver.
      *
      * See @ref workingWithSignals
      */
    template <typename... Param>
    static inline void connect(const Signal<Param...> &signal, const Signal<Param...> &receiver)
    {
        const_cast<Signal<Param...>*>(&signal)->connect(receiver);
    }

    /**
      * Connects @p signal to the static method or function @p member.
      *
      * See @ref workingWithSignals
      */
    template <typename Member, typename... Param>
    static inline void connectStatic(const Signal<Param...> &signal, Member member)
    {
        const_cast<Signal<Param...>*>(&signal)->connectStatic(member);
    }

    /**
      * Connects @p signal to the static multi method or function @p member.
      *
      * See @ref workingWithSignals
      */
    template <typename Member, typename... Param>
    static inline void connectStaticMulti(const Signal<Param...> &signal, Member member)
    {
        const_cast<Signal<Param...>*>(&signal)->connectStaticMulti(member);
    }

    /**
      * Disconnects @p signal from @p member on @p receiver.
      *
      * @note It is possible to connect the same signal to the same member and
      *       receiver more than one time. This will disconnect only the first
      *       match, not all of them.
      *
      * See @ref workingWithSignals
      */
    template <typename Receiver, typename Member, typename... Param>
    static inline void disconnect(const Signal<Param...> &signal, Receiver *receiver, Member member)
    {
        const_cast<Signal<Param...>*>(&signal)->disconnect(receiver, member);
    }

    /**
      * Disconnects @p signal from multi @p member on @p receiver.
      *
      * @note It is possible to connect the same signal to the same member and
      *       receiver more than one time. This will disconnect only the first
      *       match, not all of them.
      *
      * See @ref workingWithSignals
      */
    template <typename Receiver, typename Member, typename... Param>
    static inline void disconnectMulti(const Signal<Param...> &signal, Receiver *receiver, Member member)
    {
        const_cast<Signal<Param...>*>(&signal)->disconnectMulti(receiver, member);
    }

    /**
      * Removes the signal forwarding of @p signal to @p receiver.
      *
      * @note It is possible to forward the same signal to the same signal
      *       more than one time. This will disconnect only the first match,
      *       not all of them.
      *
      * See @ref workingWithSignals
      */
    template <typename... Param>
    static inline void disconnect(const Signal<Param...> &signal, const Signal<Param...> &receiver)
    {
        const_cast<Signal<Param...>*>(&signal)->disconnect(receiver);
    }

    /**
      * Disconnects @p signal from static method or function @p member.
      *
      * @note It is possible to connect the same signal to the same static
      *       method or function more than one time. This will disconnect
      *       only the first match, not all of them.
      *
      * See @ref workingWithSignals
      */
    template <typename Member, typename... Param>
    static inline void disconnectStatic(const Signal<Param...> &signal, Member member)
    {
        const_cast<Signal<Param...>*>(&signal)->disconnectStatic(member);
    }

    /**
      * Disconnects @p signal from multi static method or function @p member.
      *
      * @note It is possible to connect the same signal to the same static
      *       method or function more than one time. This will disconnect
      *       only the first match, not all of them.
      *
      * See @ref workingWithSignals
      */
    template <typename Member, typename... Param>
    static inline void disconnectStaticMulti(const Signal<Param...> &signal, Member member)
    {
        const_cast<Signal<Param...>*>(&signal)->disconnectStaticMulti(member);
    }

    /**
      * Disconnects @p signal completely from @p receiver.
      *
      * See @ref workingWithSignals
      */
    template <typename Receiver, typename... Param>
    static inline void disconnect(const Signal<Param...> &signal, Receiver *receiver)
    {
        const_cast<Signal<Param...>*>(&signal)->disconnect(receiver);
    }

    /**
      * All signals in @p sender will become disconnected from their receivers.
      *
      * @note This cannot be undone. If you want to temporarily fake this effect, use setEmitBlocked.
      *
      * See @ref workingWithSignals
      */
    static void disconnectSender(Object *sender);

    /**
      * All signals connected to @p receiver will be disconnected from it.
      *
      * @note If signals were connected to other receivers, those are still connected.
      *
      * @note This cannot be undone. If you want to temporarily fake this effect, use setBlockedSignals.
      *
      * See @ref workingWithSignals
      */
    static void disconnectReceiver(Object *receiver);

    /**
      * Equivalent to disconnectSender() and disconnectReceiver() on @p object.
      *
      * See @ref workingWithSignals
      */
    static void fullyDisconnect(Object *object);

    /**
      * Deletes this object right now.
      */
    void deleteNow();

    /**
      * Deletes this object on the next event loop.
      */
    void deleteLater();

    /**
      * @internal
      * @note Defined for binary compatibility reasons.
      */
    virtual void *virtual_hook(int id, void *param);

protected:
    /**
      * Emits @p signal with parameters @p param.
      */
    template <typename... Param>
    void inline emit(const Signal<Param...> &signal, const Param&... param) const
    {
        const_cast<Signal<Param...>*>(&signal)->emit(param...);
    }

    /**
      * @internal
      */
    virtual void signalCreated(SignalBase *signal);

    /**
      * @internal
      */
    virtual void signalConnected(SignalBase *signal);

    /**
      * @internal
      */
    virtual void signalDisconnected(SignalBase *signal);

    /**
      * @internal
      */
    virtual List<SignalBase*> signals() const;

private:
    /**
      * @internal
      */
    Object();

    class Private;
    Private *const d;

public:
    IDEAL_SIGNAL(destroyed);
};

}

#endif //OBJECT_H
