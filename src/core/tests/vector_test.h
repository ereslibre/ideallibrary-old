/*
 * This file is part of the Ideal Library
 * Copyright (C) 2010 Rafael Fernández López <ereslibre@ereslibre.es>
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

#ifndef VECTOR_TEST_H
#define VECTOR_TEST_H

#include <cppunit/extensions/HelperMacros.h>

class VectorTest
    : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(VectorTest);
    CPPUNIT_TEST(constructor);
    CPPUNIT_TEST(append);
    CPPUNIT_TEST(prepend);
    CPPUNIT_TEST(insertAt);
    CPPUNIT_TEST(removeAt);
    CPPUNIT_TEST(clear);
    CPPUNIT_TEST(size);
    CPPUNIT_TEST(operatorAt);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

    void constructor();
    void append();
    void prepend();
    void insertAt();
    void removeAt();
    void clear();
    void size();
    void operatorAt();
};

#endif //VECTOR_TEST_H
