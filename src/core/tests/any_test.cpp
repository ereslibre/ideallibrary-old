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

#include "any_test.h"

#include <core/any.h>
#include <core/uri.h>

#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

using namespace IdealCore;

CPPUNIT_TEST_SUITE_REGISTRATION(AnyTest);

void AnyTest::setUp()
{
}

void AnyTest::tearDown()
{
}

struct MyStruct {
    bool operator==(const MyStruct &myStruct) const
    {
        return a == myStruct.a && b == myStruct.b && c == myStruct.c && d == myStruct.d;
    }

    String a;
    Uri    b;
    int    c;
    float  d;
};

void AnyTest::get()
{
    {
        int a = 100;
        Any b(a);
        CPPUNIT_ASSERT_EQUAL(100, b.get<int>());
    }
    {
        float a = 100.0;
        Any b(a);
        CPPUNIT_ASSERT_EQUAL((float) 100.0, b.get<float>());
    }
    {
        MyStruct s;
        s.a = "Hello";
        s.b = "http://www.google.com";
        s.c = 100;
        s.d = 50.55;
        Any as(s);
        CPPUNIT_ASSERT_EQUAL(String("Hello"), as.get<MyStruct>().a);
        CPPUNIT_ASSERT_EQUAL(Uri("http://www.google.com"), as.get<MyStruct>().b);
        CPPUNIT_ASSERT_EQUAL(100, as.get<MyStruct>().c);
        CPPUNIT_ASSERT_EQUAL((float) 50.55, as.get<MyStruct>().d);
    }
}

void AnyTest::typeName()
{
}

void AnyTest::operatorEquals()
{
}

void AnyTest::operatorEqualsEquals()
{
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
