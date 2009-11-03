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

#include "char_test.h"

#include <core/char.h>

#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

using namespace IdealCore;

CPPUNIT_TEST_SUITE_REGISTRATION(CharTest);

void CharTest::setUp()
{
}

void CharTest::tearDown()
{
}

void CharTest::octetsRequired()
{
    {
        Char c('a');
        CPPUNIT_ASSERT_EQUAL(1, c.octetsRequired());
    }
    {
        Char c(L'a');
        CPPUNIT_ASSERT_EQUAL(1, c.octetsRequired());
    }
    {
        Char c(L'ñ');
        CPPUNIT_ASSERT_EQUAL(2, c.octetsRequired());
    }
    {
        Char c(L'€');
        CPPUNIT_ASSERT_EQUAL(3, c.octetsRequired());
    }
    {
        Char c(L'𝚿');
        CPPUNIT_ASSERT_EQUAL(4, c.octetsRequired());
    }
}

void CharTest::operatorChar()
{
    {
        Char c('a');
        char c1 = c;
        CPPUNIT_ASSERT_EQUAL('a', c1);
    }
    {
        Char c(L'ñ');
        char c1 = c;
        CPPUNIT_ASSERT(((unsigned int) L'ñ') != (unsigned int) c1);
    }
    {
        Char c(L'€');
        char c1 = c;
        CPPUNIT_ASSERT(((unsigned int) L'€') != (unsigned int) c1);
    }
    {
        Char c(L'𝚿');
        char c1 = c;
        CPPUNIT_ASSERT(((unsigned int) L'𝚿') != (unsigned int) c1);
    }
}

void CharTest::operatorUnsignedShort()
{
    {
        Char c('a');
        unsigned short c1 = c;
        CPPUNIT_ASSERT_EQUAL((unsigned short) L'a', c1);
    }
    {
        Char c(L'ñ');
        unsigned short c1 = c;
        CPPUNIT_ASSERT_EQUAL((unsigned short) L'ñ', c1);
    }
    {
        Char c(L'€');
        unsigned short c1 = c;
        CPPUNIT_ASSERT_EQUAL((unsigned short) L'€', c1);
    }
    {
        Char c(L'𝚿');
        unsigned short c1 = c;
        CPPUNIT_ASSERT(((unsigned int) L'𝚿') != (unsigned int) c1);
    }
}

void CharTest::operatorUnsignedInt()
{
    {
        Char c('a');
        unsigned int c1 = c;
        CPPUNIT_ASSERT_EQUAL((unsigned int) L'a', c1);
    }
    {
        Char c(L'ñ');
        unsigned int c1 = c;
        CPPUNIT_ASSERT_EQUAL((unsigned int) L'ñ', c1);
    }
    {
        Char c(L'€');
        unsigned int c1 = c;
        CPPUNIT_ASSERT_EQUAL((unsigned int) L'€', c1);
    }
    {
        Char c(L'𝚿');
        unsigned int c1 = c;
        CPPUNIT_ASSERT_EQUAL((unsigned int) L'𝚿', c1);
    }
}

void CharTest::operatorEqualsChar()
{
    {
        Char c('a');
        Char c1('a');
        CPPUNIT_ASSERT(c == c1);
    }
    {
        Char c(L'ñ');
        Char c1(L'ñ');
        CPPUNIT_ASSERT(c == c1);
    }
    {
        Char c(L'€');
        Char c1(L'€');
        CPPUNIT_ASSERT(c == c1);
    }
    {
        Char c(L'𝚿');
        Char c1(L'𝚿');
        CPPUNIT_ASSERT(c == c1);
    }
}

void CharTest::operatorEqualsCharNative()
{
    {
        Char c('a');
        char c1 = 'a';
        CPPUNIT_ASSERT(c == c1);
    }
}

void CharTest::operatorEqualsUnsignedShort()
{
    {
        Char c('a');
        unsigned short c1 = 'a';
        CPPUNIT_ASSERT(c == c1);
    }
    {
        Char c(L'ñ');
        unsigned short c1 = L'ñ';
        CPPUNIT_ASSERT(c == c1);
    }
    {
        Char c(L'€');
        unsigned short c1 = L'€';
        CPPUNIT_ASSERT(c == c1);
    }
}

void CharTest::operatorEqualsUnsignedInt()
{
    {
        Char c('a');
        unsigned int c1 = 'a';
        CPPUNIT_ASSERT(c == c1);
    }
    {
        Char c(L'ñ');
        unsigned int c1 = L'ñ';
        CPPUNIT_ASSERT(c == c1);
    }
    {
        Char c(L'€');
        unsigned int c1 = L'€';
        CPPUNIT_ASSERT(c == c1);
    }
    {
        Char c(L'𝚿');
        unsigned int c1 = L'𝚿';
        CPPUNIT_ASSERT(c == c1);
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
