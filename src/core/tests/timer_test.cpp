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

#include "timer_test.h"

#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <core/timer.h>
#include <core/application.h>

#include <stdlib.h>
#include <sys/wait.h>

using namespace IdealCore;

static iint32  res;
static iint32  s_argc;
static ichar **s_argv;

CPPUNIT_TEST_SUITE_REGISTRATION(TimerTest);

void TimerTest::setUp()
{
}

void TimerTest::tearDown()
{
}

static bool didTimeout = false;
static void timerTimeout()
{
    didTimeout = true;
}

void TimerTest::nullInterval()
{
    const pid_t pid = fork();
    if (!pid) {
        Application app(s_argc, s_argv);
        Timer timer(&app);
        Object::connectStatic(timer.timeout, timerTimeout);
        Object::connect(timer.timeout, &app, &Application::quit);
        timer.setInterval(0);
        timer.start();
        app.exec();
        CPPUNIT_ASSERT(didTimeout);
    } else {
        waitpid(pid, &res, 0);
    }
}

void TimerTest::secondInterval()
{
    const pid_t pid = fork();
    if (!pid) {
        Application app(s_argc, s_argv);
        Timer timer(&app);
        Object::connectStatic(timer.timeout, timerTimeout);
        Object::connect(timer.timeout, &app, &Application::quit);
        timer.setInterval(1000);
        timer.start();
        CPPUNIT_ASSERT(!didTimeout);
        app.exec();
        CPPUNIT_ASSERT(didTimeout);
    } else {
        waitpid(pid, &res, 0);
    }
}

static iint32 didTimeout1 = 0;
static iint32 didTimeout2 = 0;

static void timeout1()
{
    didTimeout1 += 1;
    CPPUNIT_ASSERT(abs(didTimeout1 - didTimeout2) <= 2);
}

static void timeout2()
{
    didTimeout2 += 2;
    CPPUNIT_ASSERT(abs(didTimeout1 - didTimeout2) <= 2);
}

void TimerTest::testLoops()
{
    const pid_t pid = fork();
    if (!pid) {
        Application app(s_argc, s_argv);
        Timer timer1(&app);
        Object::connectStatic(timer1.timeout, timeout1);
        timer1.setInterval(1000);
        timer1.start(Timer::NoSingleShot);
        Timer timer2(&app);
        Object::connectStatic(timer2.timeout, timeout2);
        timer2.setInterval(2000);
        timer2.start(Timer::NoSingleShot);
        Timer timer3(&app);
        Object::connect(timer3.timeout, &app, &Application::quit);
        timer3.setInterval(10000);
        timer3.start();
        app.exec();
        CPPUNIT_ASSERT_EQUAL(10, didTimeout1);
        CPPUNIT_ASSERT_EQUAL(10, didTimeout2);
    } else {
        waitpid(pid, &res, 0);
    }
}

static Timer *staticTimer = 0;
static bool unreachableReached = false;

static void timeout3()
{
    if (staticTimer->state() == Timer::Stopped) {
        staticTimer->start();
    } else {
        staticTimer->stop();
    }
}

static void unreachable()
{
    unreachableReached = true;
    CPPUNIT_ASSERT(false); // this should never happen
}

void TimerTest::testStop()
{
    const pid_t pid = fork();
    if (!pid) {
        Application app(s_argc, s_argv);
        staticTimer = new Timer(&app);
        Object::connectStatic(staticTimer->timeout, unreachable);
        staticTimer->setInterval(600);
        staticTimer->start();
        Timer timer1(&app);
        Object::connectStatic(timer1.timeout, timeout3);
        timer1.setInterval(500);
        timer1.start(Timer::NoSingleShot);
        Timer timer2(&app);
        Object::connect(timer2.timeout, &app, &Application::quit);
        timer2.setInterval(10000);
        timer2.start();
        app.exec();
        CPPUNIT_ASSERT(!unreachableReached);
    } else {
        waitpid(pid, &res, 0);
    }
}

int main(int argc, char **argv)
{
    s_argc = argc;
    s_argv = argv;
    IDEAL_SDEBUG("*** This test will take 21 seconds at least. Please, be patient");

    CppUnit::Test *suite = CppUnit::TestFactoryRegistry::getRegistry().makeTest();

    CppUnit::TextUi::TestRunner runner;
    runner.addTest(suite);

    runner.setOutputter(new CppUnit::CompilerOutputter(&runner.result(), std::cerr));
    bool wasSuccessful = runner.run();

    return wasSuccessful ? 0 : 1;
}
