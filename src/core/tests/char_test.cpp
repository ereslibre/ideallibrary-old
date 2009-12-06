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

void CharTest::constructor()
{
    {
        Char c("a");
        CPPUNIT_ASSERT_EQUAL(Char('a'), c);
    }
    {
        Char c("ñ");
        CPPUNIT_ASSERT_EQUAL(Char(L'ñ'), c);
    }
    {
        Char c("โ");
        CPPUNIT_ASSERT_EQUAL(Char(L'โ'), c);
    }
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
        ichar c1 = c;
        CPPUNIT_ASSERT_EQUAL('a', c1);
    }
    {
        Char c(L'ñ');
        ichar c1 = c;
        CPPUNIT_ASSERT(((iuint32) L'ñ') != (iuint32) c1);
    }
    {
        Char c(L'€');
        ichar c1 = c;
        CPPUNIT_ASSERT(((iuint32) L'€') != (iuint32) c1);
    }
    {
        Char c(L'𝚿');
        ichar c1 = c;
        CPPUNIT_ASSERT(((iuint32) L'𝚿') != (iuint32) c1);
    }
}

void CharTest::operatorUnsignedShort()
{
    {
        Char c('a');
        iuint16 c1 = c;
        CPPUNIT_ASSERT_EQUAL((iuint16) L'a', c1);
    }
    {
        Char c(L'ñ');
        iuint16 c1 = c;
        CPPUNIT_ASSERT_EQUAL((iuint16) L'ñ', c1);
    }
    {
        Char c(L'€');
        iuint16 c1 = c;
        CPPUNIT_ASSERT_EQUAL((iuint16) L'€', c1);
    }
    {
        Char c(L'𝚿');
        iuint16 c1 = c;
        CPPUNIT_ASSERT(((iuint32) L'𝚿') != (iuint32) c1);
    }
}

void CharTest::operatorUnsignedInt()
{
    {
        Char c('a');
        iuint32 c1 = c;
        CPPUNIT_ASSERT_EQUAL((iuint32) L'a', c1);
    }
    {
        Char c(L'ñ');
        iuint32 c1 = c;
        CPPUNIT_ASSERT_EQUAL((iuint32) L'ñ', c1);
    }
    {
        Char c(L'€');
        iuint32 c1 = c;
        CPPUNIT_ASSERT_EQUAL((iuint32) L'€', c1);
    }
    {
        Char c(L'𝚿');
        iuint32 c1 = c;
        CPPUNIT_ASSERT_EQUAL((iuint32) L'𝚿', c1);
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
        ichar c1 = 'a';
        CPPUNIT_ASSERT(c == c1);
    }
}

void CharTest::operatorEqualsUnsignedShort()
{
    {
        Char c('a');
        iuint16 c1 = 'a';
        CPPUNIT_ASSERT(c == c1);
    }
    {
        Char c(L'ñ');
        iuint16 c1 = L'ñ';
        CPPUNIT_ASSERT(c == c1);
    }
    {
        Char c(L'€');
        iuint16 c1 = L'€';
        CPPUNIT_ASSERT(c == c1);
    }
}

void CharTest::operatorEqualsUnsignedInt()
{
    {
        Char c('a');
        iuint32 c1 = 'a';
        CPPUNIT_ASSERT(c == c1);
    }
    {
        Char c(L'ñ');
        iuint32 c1 = L'ñ';
        CPPUNIT_ASSERT(c == c1);
    }
    {
        Char c(L'€');
        iuint32 c1 = L'€';
        CPPUNIT_ASSERT(c == c1);
    }
    {
        Char c(L'𝚿');
        iuint32 c1 = L'𝚿';
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
