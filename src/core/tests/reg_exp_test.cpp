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

#include "reg_exp_test.h"

#include <core/reg_exp.h>

#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

using namespace IdealCore;

CPPUNIT_TEST_SUITE_REGISTRATION(RegExpTest);

void RegExpTest::setUp()
{
}

void RegExpTest::tearDown()
{
}

void RegExpTest::match()
{
    {
        RegExp r("(ab)*(\\d+)(\\w*)");
        CPPUNIT_ASSERT(r.match("1"));
        CPPUNIT_ASSERT(r.match("abab1234Hello"));
    }
    {
        RegExp r("ñ*");
        CPPUNIT_ASSERT(r.match("ñññ"));
    }
    {
        RegExp r("€+");
        CPPUNIT_ASSERT(r.match("€€€"));
    }
}

void RegExpTest::numCaptures()
{
    {
        RegExp r("(ab)*(\\d+)(\\w*)");
        CPPUNIT_ASSERT(r.match("1"));
        CPPUNIT_ASSERT_EQUAL((size_t) 3, r.numCaptures());
    }
    {
        RegExp r("ñ*");
        CPPUNIT_ASSERT(r.match("ñññ"));
        CPPUNIT_ASSERT_EQUAL((size_t) 0, r.numCaptures());
    }
    {
        RegExp r("€+");
        CPPUNIT_ASSERT(r.match("€€€"));
        CPPUNIT_ASSERT_EQUAL((size_t) 0, r.numCaptures());
    }
    {
        RegExp r("(€+)");
        CPPUNIT_ASSERT(r.match("€€€"));
        CPPUNIT_ASSERT_EQUAL((size_t) 1, r.numCaptures());
    }
}

void RegExpTest::getCapture()
{
    {
        RegExp r("(ab)*(\\d+)(\\w*)");
        CPPUNIT_ASSERT(r.match("1"));
        CPPUNIT_ASSERT_EQUAL(String(), r.getCapture(0));
        CPPUNIT_ASSERT_EQUAL(String("1"), r.getCapture(1));
        CPPUNIT_ASSERT_EQUAL(String(), r.getCapture(2));
    }
    {
        RegExp r("(€+)");
        CPPUNIT_ASSERT(r.match("€€€"));
        CPPUNIT_ASSERT_EQUAL(String("€€€"), r.getCapture(0));
    }
}

void RegExpTest::operatorEquals()
{
    {
        RegExp r("a*");
        RegExp s("b*");
        s = r;
        CPPUNIT_ASSERT(s.match("aaa"));
    }
    {
        String r("a*");
        RegExp s("b*");
        s = r;
        CPPUNIT_ASSERT(s.match("aaa"));
    }
}

void RegExpTest::operatorEqualsEquals()
{
    {
        RegExp r("a*");
        RegExp s("a*");
        CPPUNIT_ASSERT(r == s);
    }
    {
        RegExp r("a*");
        RegExp s("b*");
        s = r;
        CPPUNIT_ASSERT(r == s);
    }
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
