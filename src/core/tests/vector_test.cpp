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

#include "vector_test.h"

#include <core/vector.h>

#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

using namespace IdealCore;

CPPUNIT_TEST_SUITE_REGISTRATION(VectorTest);

void VectorTest::setUp()
{
}

void VectorTest::tearDown()
{
}

void VectorTest::constructor()
{
    Vector<size_t> v;
    CPPUNIT_ASSERT(v.isEmpty());
}

void VectorTest::append()
{
    {
        Vector<size_t> v;

        for (size_t i = 0; i <= 100; ++i) {
            v.append(i);
        }
        for (size_t i = 0; i <= 100; ++i) {
            CPPUNIT_ASSERT_EQUAL(i, v[i]);
        }
    }
    // Check operator<<
    {
        Vector<size_t> v;
        v << 1 << 2 << 3 << 4 << 5;
        CPPUNIT_ASSERT_EQUAL((size_t) 5, v.size());
        for (size_t i = 1; i <= 5; ++i) {
            CPPUNIT_ASSERT_EQUAL(i, v[i - 1]);
        }
    }
}

void VectorTest::prepend()
{
    {
        Vector<size_t> v;

        for (size_t i = 0; i <= 100; ++i) {
            v.prepend(i);
        }
        for (size_t i = 0; i <= 100; ++i) {
            CPPUNIT_ASSERT_EQUAL((size_t) (100 - i), v[i]);
        }
    }
}

void VectorTest::insertAt()
{
    // Insert always at end of vector
    {
        Vector<size_t> v;

        for (size_t i = 0; i <= 100; ++i) {
            v.insertAt(i, v.size());
        }
        for (size_t i = 0; i <= 100; ++i) {
            CPPUNIT_ASSERT_EQUAL(i, v[i]);
        }
    }
    // Insert always at beginning of vector
    {
        Vector<size_t> v;

        for (size_t i = 0; i <= 100; ++i) {
            v.insertAt(i, 0);
        }
        for (size_t i = 0; i <= 100; ++i) {
            CPPUNIT_ASSERT_EQUAL((size_t) (100 - i), v[i]);
        }
    }
}

void VectorTest::removeAt()
{
    {
        Vector<size_t> v;

        // Insert always at end of vector
        {
            for (size_t i = 0; i <= 100; ++i) {
                v.insertAt(i, v.size());
            }
        }
        // Remove them by deleting always the first one
        {
            for (size_t i = 0; i <= 100; ++i) {
                v.removeAt(0);
                for (size_t j = 0; j < 100 - i; ++j) {
                    CPPUNIT_ASSERT_EQUAL(j + i + 1, v[j]);
                }
            }
        }
    }
    {
        Vector<size_t> v;

        // Insert always at beginning of vector
        {
            for (size_t i = 0; i <= 100; ++i) {
                v.insertAt(i, 0);
            }
        }
        // Remove them by deleting always the first one
        {
            for (size_t i = 0; i <= 100; ++i) {
                v.removeAt(0);
                for (size_t j = 0; j < 100 - i; ++j) {
                    CPPUNIT_ASSERT_EQUAL(100 - (j + i + 1), v[j]);
                }
            }
        }
    }
    // Add an element very far (even further than the container size)
    {
        Vector<size_t> v;

        for (size_t i = 0; i <= 100; ++i) {
            v.insertAt(i, v.size());
        }

        CPPUNIT_ASSERT_EQUAL((size_t) 101, v.size());
        CPPUNIT_ASSERT_EQUAL((size_t) 101, v.count());

        v.insertAt(50, 20001);

        CPPUNIT_ASSERT_EQUAL((size_t) 20002, v.size());
        CPPUNIT_ASSERT_EQUAL((size_t) 102, v.count());

        for (size_t i = 0; i <= 100; ++i) {
            CPPUNIT_ASSERT_EQUAL(i, v[i]);
        }

        CPPUNIT_ASSERT_EQUAL((size_t) 0, v[10000]);
        CPPUNIT_ASSERT_EQUAL((size_t) 0, v[20000]);
        CPPUNIT_ASSERT_EQUAL((size_t) 50, v[20001]);
        CPPUNIT_ASSERT_EQUAL((size_t) 0, v[20002]);

        for (size_t i = 0; i <= 100; ++i) {
            v.removeAt(0);
        }

        CPPUNIT_ASSERT_EQUAL((size_t) 19901, v.size());
        CPPUNIT_ASSERT_EQUAL((size_t) 1, v.count());
    }
}

void VectorTest::clear()
{
}

void VectorTest::size()
{
    {
        Vector<size_t> v;

        CPPUNIT_ASSERT_EQUAL((size_t) 0, v.size());
    }
    {
        Vector<size_t> v;

        // Insert 101 elements
        {
            for (size_t i = 0; i <= 100; ++i) {
                v.insertAt(i, v.size());
            }

            CPPUNIT_ASSERT_EQUAL((size_t) 101, v.size());
        }
        // Remove 51 elements
        {
            for (size_t i = 0; i <= 50; ++i) {
                v.removeAt(i);
            }

            CPPUNIT_ASSERT_EQUAL((size_t) 50, v.size());
        }
        // Remove 50 elements
        {
            for (size_t i = 0; i < 50; ++i) {
                v.removeAt(0);
            }

            CPPUNIT_ASSERT_EQUAL((size_t) 0, v.size());
            CPPUNIT_ASSERT(v.isEmpty());
        }
        // Remove another element (the vector is already empty)
        {
            v.removeAt(0);
            CPPUNIT_ASSERT_EQUAL((size_t) 0, v.size());
            CPPUNIT_ASSERT(v.isEmpty());
        }
    }
}

void VectorTest::operatorAt()
{
    // Access to a non-existant index
    {
        Vector<size_t> v;

        CPPUNIT_ASSERT_EQUAL((size_t) 0, v[100]);
    }
    {
        Vector<size_t> v;

        // Insert always at end of vector
        {
            for (size_t i = 0; i <= 100; ++i) {
                v.insertAt(i, v.size());
            }
        }

        CPPUNIT_ASSERT_EQUAL((size_t) 100, v[100]);
        CPPUNIT_ASSERT_EQUAL((size_t) 0, v[101]);
        CPPUNIT_ASSERT_EQUAL((size_t) 0, v[-1]);
    }
    // Make sure the user cannot override the default constructed value for invalid indexes
    {
        Vector<size_t> v;

        size_t &element = v[0];
        CPPUNIT_ASSERT_EQUAL((size_t) 0, element);
        element = 5;

        size_t &element2 = v[1];
        CPPUNIT_ASSERT_EQUAL((size_t) 0, element2);
    }
    // Make sure we are detaching when returning non-const references
    {
        Vector<size_t> v;
        v.append(5);

        Vector<size_t> v2(v);

        size_t &n = v[0];
        n = 10;

        CPPUNIT_ASSERT_EQUAL((size_t) 10, v[0]);
        CPPUNIT_ASSERT_EQUAL((size_t) 5, v2[0]);
    }
}

void VectorTest::iterators()
{
    {
        Vector<size_t> v;

        for (size_t i = 0; i <= 100; ++i) {
            v.insertAt(i, v.size());
        }

        Vector<size_t> v2(v);

        {
            size_t i = 0;
            Vector<size_t>::Iterator it(v);
            while (it.hasNext()) {
                size_t &j = it.next();
                CPPUNIT_ASSERT_EQUAL(i, j);
                j = 100;
                ++i;
            }
            CPPUNIT_ASSERT_EQUAL((size_t) 101, i);
        }

        {
            size_t i = 0;
            Vector<size_t>::Iterator it(v2);
            while (it.hasNext()) {
                CPPUNIT_ASSERT_EQUAL(i, it.next());
                ++i;
            }
            CPPUNIT_ASSERT_EQUAL((size_t) 101, i);
        }
    }
    {
        Vector<size_t> v;

        for (size_t i = 0; i < 100; ++i) {
            v.insertAt(i, v.size());
        }

        Vector<size_t>::Iterator it(v);
        while (it.hasNext()) {
            size_t &i = it.next();
            if (i >= 50 && i < 70) {
                it.remove();
            }
        }

        it.rewind();

        size_t i = 0;
        while (it.hasNext()) {
            size_t &j = it.next();
            if (i == 50) {
                i = 70;
            }
            CPPUNIT_ASSERT_EQUAL(i, j);
            ++i;
        }

        CPPUNIT_ASSERT_EQUAL((size_t) 80, v.size());
    }

    {
        Vector<size_t> v;

        for (size_t i = 0; i < 100; ++i) {
            v.insertAt(i, v.size());
        }

        Vector<size_t>::Iterator it(v);
        while (it.hasNext()) {
            if (it.next() % 2) {
                it.remove();
            }
        }

        it.rewind();

        size_t i = 0;
        while (it.hasNext()) {
            size_t &j = it.next();
            CPPUNIT_ASSERT_EQUAL(i, j);
            i += 2;
        }

        CPPUNIT_ASSERT_EQUAL((size_t) 50, v.size());
    }
    {
        Vector<size_t> v;

        // Fill with 0, 1, 3, 4
        for (size_t i = 0; i < 5; ++i) {
            if (i != 2) {
                v.insertAt(i, v.size());
            }
        }

        Vector<size_t>::Iterator it(v, 1);
        it.next();
        it.insertAfter(2);

        Vector<size_t>::Iterator it2(v);
        size_t i = 0;
        while (it2.hasNext()) {
            CPPUNIT_ASSERT_EQUAL(i, it2.next());
            ++i;
        }
    }
    {
        Vector<size_t> v;

        // Fill with 0, 1, 3, 4
        for (size_t i = 0; i < 5; ++i) {
            if (i != 2) {
                v.insertAt(i, v.size());
            }
        }

        Vector<size_t>::Iterator it(v, 2);
        it.next();
        it.insertBefore(2);

        Vector<size_t>::Iterator it2(v);
        size_t i = 0;
        while (it2.hasNext()) {
            CPPUNIT_ASSERT_EQUAL(i, it2.next());
            ++i;
        }
    }
    {
        Vector<size_t> v;
        v.insertAt(5, 0);

        Vector<size_t>::Iterator it(v);
        it.next();
        it.insertAfter(10);

        CPPUNIT_ASSERT(it.hasNext());
        CPPUNIT_ASSERT_EQUAL((size_t) 10, it.next());
        CPPUNIT_ASSERT(!it.hasNext());
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
