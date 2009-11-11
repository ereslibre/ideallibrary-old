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

#include "application_test.h"

#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <core/application.h>

#include <stdlib.h>

using namespace IdealCore;

CPPUNIT_TEST_SUITE_REGISTRATION(ApplicationTest);

void ApplicationTest::setUp()
{
}

void ApplicationTest::tearDown()
{
}

void ApplicationTest::testOptionHandling()
{
    optind = 1;
    const ichar *argv[] = {"app", "-a", "--force", "foo.txt", "bar.txt"};
    Application *instance = new Application(5, (ichar**) argv);
    Option allFilesOption;
    instance->addOptionWithoutArg(allFilesOption, 'a', "all");
    Option forceOption;
    instance->addOptionWithoutArg(forceOption, 'f', "force");
    Option hiddenOption;
    instance->addOptionWithoutArg(hiddenOption, 'h', "hidden");
    List<ichar*> res = instance->parseOptions();
    CPPUNIT_ASSERT(allFilesOption.isValid());
    CPPUNIT_ASSERT(allFilesOption.isOptSet());
    CPPUNIT_ASSERT(forceOption.isValid());
    CPPUNIT_ASSERT(forceOption.isOptSet());
    CPPUNIT_ASSERT(hiddenOption.isValid());
    CPPUNIT_ASSERT(!hiddenOption.isOptSet());
    CPPUNIT_ASSERT_EQUAL((size_t) 2, res.size());
    delete instance;
}

void ApplicationTest::testNullOptionsHandling()
{
    optind = 1;
    const ichar *argv[] = {"app", "foo.txt"};
    Application *instance = new Application(2, (ichar**) argv);
    List<ichar*> res = instance->parseOptions();
    CPPUNIT_ASSERT_EQUAL((size_t) 1, res.size());
    delete instance;
}

void ApplicationTest::testInvalidOptionsHandling()
{
    optind = 1;
    const ichar *argv[] = {"app", "--non-existant", "foo.txt"};
    Application *instance = new Application(3, (ichar**) argv);
    List<ichar*> res = instance->parseOptions();
    CPPUNIT_ASSERT_EQUAL((size_t) 1, res.size());
    delete instance;
}

static bool receivedUnknownOption = false;
static void receiveInvalidness()
{
    receivedUnknownOption = true;
}

void ApplicationTest::testInvalidLongOption()
{
    optind = 1;
    receivedUnknownOption = false;
    const ichar *argv[] = {"app", "-non-existant", "foo.txt"};
    Application *instance = new Application(3, (ichar**) argv);
    Object::connectStatic(instance->invalidOption, receiveInvalidness);
    List<ichar*> res = instance->parseOptions();
    CPPUNIT_ASSERT(receivedUnknownOption);
    CPPUNIT_ASSERT_EQUAL((size_t) 1, res.size());
    delete instance;
}

void ApplicationTest::testInvalidShortOption()
{
    optind = 1;
    receivedUnknownOption = false;
    const ichar *argv[] = {"app", "-rnf", "foo.txt"};
    Application *instance = new Application(3, (ichar**) argv);
    Option recursiveOption;
    instance->addOptionWithoutArg(recursiveOption, 'r', "recursive");
    Option forceOption;
    instance->addOptionWithoutArg(forceOption, 'f', "force");
    Object::connectStatic(instance->invalidOption, receiveInvalidness);
    List<ichar*> res = instance->parseOptions(Application::Strict);
    CPPUNIT_ASSERT(receivedUnknownOption);
    CPPUNIT_ASSERT(recursiveOption.isOptSet());
    CPPUNIT_ASSERT(forceOption.isOptSet()); // note this
    CPPUNIT_ASSERT_EQUAL((size_t) 1, res.size());
    delete instance;
}

void ApplicationTest::testValidLongOption()
{
    optind = 1;
    const ichar *argv[] = {"app", "-a", "-hidden"};
    Application *instance = new Application(3, (ichar**) argv);
    Option allFilesOption;
    instance->addOptionWithoutArg(allFilesOption, 'a', "all");
    Option forceOption;
    instance->addOptionWithoutArg(forceOption, 'f', "force");
    Option hiddenOption;
    instance->addOptionWithoutArg(hiddenOption, 'h', "hidden");
    List<ichar*> res = instance->parseOptions();
    CPPUNIT_ASSERT(allFilesOption.isValid());
    CPPUNIT_ASSERT(allFilesOption.isOptSet());
    CPPUNIT_ASSERT(forceOption.isValid());
    CPPUNIT_ASSERT(!forceOption.isOptSet());
    CPPUNIT_ASSERT(hiddenOption.isValid());
    CPPUNIT_ASSERT(hiddenOption.isOptSet());
    CPPUNIT_ASSERT_EQUAL((size_t) 0, res.size());
    delete instance;
}

void ApplicationTest::testStrict()
{
    optind = 1;
    const ichar *argv[] = {"app", "-a", "-test"};
    Application *instance = new Application(3, (ichar**) argv);
    Option allFilesOption;
    instance->addOptionWithoutArg(allFilesOption, 'a', "all");
    Option forceOption;
    instance->addOptionWithoutArg(forceOption, 'f', "force");
    Option hiddenOption;
    instance->addOptionWithoutArg(hiddenOption, 'h', "test");
    List<ichar*> res = instance->parseOptions(Application::Strict);
    CPPUNIT_ASSERT(allFilesOption.isValid());
    CPPUNIT_ASSERT(allFilesOption.isOptSet());
    CPPUNIT_ASSERT(forceOption.isValid());
    CPPUNIT_ASSERT(!forceOption.isOptSet());
    CPPUNIT_ASSERT(hiddenOption.isValid());
    CPPUNIT_ASSERT(!hiddenOption.isOptSet()); // note this
    CPPUNIT_ASSERT_EQUAL((size_t) 0, res.size());
    delete instance;
}

void ApplicationTest::testFlexible()
{
    optind = 1;
    const ichar *argv[] = {"app", "-a", "-test"};
    Application *instance = new Application(3, (ichar**) argv);
    Option allFilesOption;
    instance->addOptionWithoutArg(allFilesOption, 'a', "all");
    Option forceOption;
    instance->addOptionWithoutArg(forceOption, 'f', "force");
    Option hiddenOption;
    instance->addOptionWithoutArg(hiddenOption, 'h', "test");
    List<ichar*> res = instance->parseOptions(Application::Flexible);
    CPPUNIT_ASSERT(allFilesOption.isValid());
    CPPUNIT_ASSERT(allFilesOption.isOptSet());
    CPPUNIT_ASSERT(forceOption.isValid());
    CPPUNIT_ASSERT(!forceOption.isOptSet());
    CPPUNIT_ASSERT(hiddenOption.isValid());
    CPPUNIT_ASSERT(hiddenOption.isOptSet()); // note this
    CPPUNIT_ASSERT_EQUAL((size_t) 0, res.size());
    delete instance;
}

int main(int argc, char **argv)
{
    CppUnit::Test *suite = CppUnit::TestFactoryRegistry::getRegistry().makeTest();

    CppUnit::TextUi::TestRunner runner;
    runner.addTest(suite);

    runner.setOutputter(new CppUnit::CompilerOutputter(&runner.result(), std::cerr));
    bool wasSuccessful = runner.run();

    return wasSuccessful ? 0 : 1;
}
