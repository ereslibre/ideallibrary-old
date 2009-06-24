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

#include "string_test.h"

#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <core/application.h>

using namespace IdealCore;

CPPUNIT_TEST_SUITE_REGISTRATION(StringTest);

void StringTest::setUp()
{
}

void StringTest::tearDown()
{
}

void StringTest::testConstructor()
{
    String test1("Test");
    CPPUNIT_ASSERT_EQUAL((size_t) 4, test1.size());
    String test2("Tést");
    CPPUNIT_ASSERT_EQUAL((size_t) 4, test2.size());
    String test3("Tést");
    CPPUNIT_ASSERT(test2 == test3);
    String test4("Téest");
    CPPUNIT_ASSERT(test2 != test4);
    String test5_1("Teñt");
    String test5_2("Tezt");
    CPPUNIT_ASSERT(test5_1 < test5_2);
    CPPUNIT_ASSERT(test5_1 != test5_2);
    CPPUNIT_ASSERT(!(test5_1 == test5_2));
    CPPUNIT_ASSERT(test5_1 <= test5_2);
    CPPUNIT_ASSERT(test5_2 >= test5_1);
    CPPUNIT_ASSERT(test5_2 > test5_1);
}

void StringTest::testSplit()
{
    {
        String test1("No split at all");
        CPPUNIT_ASSERT_EQUAL((size_t) 1, test1.split('w').size());
    }
    {
        String test2("Option 1;Option 2");
        List<String> res = test2.split(';');
        CPPUNIT_ASSERT_EQUAL((size_t) 2, res.size());
        CPPUNIT_ASSERT_EQUAL(String("Option 1"), res.front());
        CPPUNIT_ASSERT_EQUAL(String("Option 2"), res.back());
    }
    {
        String test3(",Option 1,");
        CPPUNIT_ASSERT_EQUAL((size_t) 1, test3.split(',').size());
    }
    {
        String test4("Option 1;");
        CPPUNIT_ASSERT_EQUAL((size_t) 1, test4.split(';').size());
    }
    {
        String test5(";a;b;");
        List<String> res = test5.split(';');
        CPPUNIT_ASSERT_EQUAL((size_t) 2, res.size());
        CPPUNIT_ASSERT_EQUAL(String("a"), res.front());
        CPPUNIT_ASSERT_EQUAL(String("b"), res.back());
    }
    {
        String test6("aObocOd");
        CPPUNIT_ASSERT_EQUAL((size_t) 3, test6.split('O').size());
    }
    {
        String test7("a,b,c,d,e");
        List<String> res = test7.split(',');
        String list[] = { String("a"), String("b"), String("c"),
                          String("d"), String("e") };
        List<String>::iterator it = res.begin();
        int i = 0;
        for (it = res.begin(); it != res.end(); ++it, ++i) {
            CPPUNIT_ASSERT_EQUAL(list[i], *it);
        }
        CPPUNIT_ASSERT_EQUAL((size_t) 5, res.size());
    }
}

void StringTest::miscTests()
{
    String specialChars = returnSpecialChars();
    CPPUNIT_ASSERT_EQUAL((size_t) 10, specialChars.size());
    {
        String specialChars2(specialChars);
        CPPUNIT_ASSERT_EQUAL((size_t) 10, specialChars2.size());
    }
    {
        String specialChars;
        specialChars = returnSpecialChars();
        CPPUNIT_ASSERT_EQUAL((size_t) 10, specialChars.size());
    }
    {
        String specialChars = String('%') + String('3') + String('4');
        CPPUNIT_ASSERT_EQUAL(String("%34"), specialChars);
        CPPUNIT_ASSERT_EQUAL((size_t) 3, specialChars.size());
    }
    {
        String specialChars = String("%34");
        CPPUNIT_ASSERT_EQUAL(String("%34"), specialChars);
        CPPUNIT_ASSERT_EQUAL((size_t) 3, specialChars.size());
    }
}

String StringTest::returnSpecialChars()
{
    return "áéíóúñ€%32";
}

int main(int argc, char **argv)
{
    Application app(argc, argv);
    CppUnit::Test *suite = CppUnit::TestFactoryRegistry::getRegistry().makeTest();

    CppUnit::TextUi::TestRunner runner;
    runner.addTest(suite);

    runner.setOutputter(new CppUnit::CompilerOutputter(&runner.result(), std::cerr));
    bool wasSuccessful = runner.run();

    return wasSuccessful ? 0 : 1;
}
