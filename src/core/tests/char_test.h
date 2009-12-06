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

#ifndef CHAR_TEST_H
#define CHAR_TEST_H

#include <cppunit/extensions/HelperMacros.h>

class CharTest
    : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(CharTest);
    CPPUNIT_TEST(constructor);
    CPPUNIT_TEST(octetsRequired);
    CPPUNIT_TEST(operatorChar);
    CPPUNIT_TEST(operatorUnsignedShort);
    CPPUNIT_TEST(operatorUnsignedInt);
    CPPUNIT_TEST(operatorEquals);
    CPPUNIT_TEST(operatorEqualsEqualsChar);
    CPPUNIT_TEST(operatorEqualsEqualsCharNative);
    CPPUNIT_TEST(operatorEqualsEqualsUnsignedShort);
    CPPUNIT_TEST(operatorEqualsEqualsUnsignedInt);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

    void constructor();
    void octetsRequired();
    void operatorChar();
    void operatorUnsignedShort();
    void operatorUnsignedInt();
    void operatorEquals();
    void operatorEqualsEqualsChar();
    void operatorEqualsEqualsCharNative();
    void operatorEqualsEqualsUnsignedShort();
    void operatorEqualsEqualsUnsignedInt();
};

#endif //CHAR_TEST_H
