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

#include "bytestream_test.h"

#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <core/byte_stream.h>
#include <core/application.h>

using namespace IdealCore;

CPPUNIT_TEST_SUITE_REGISTRATION(ByteStreamTest);

void ByteStreamTest::setUp()
{
}

void ByteStreamTest::tearDown()
{
}

void ByteStreamTest::testConstructor()
{
    ByteStream *byteStream;
    {
        ichar myString[] = { "Testing" };
        byteStream = new ByteStream(myString);
    }
    CPPUNIT_ASSERT_EQUAL(String("Testing"), String(byteStream->data()));
    delete byteStream;
    {
        ichar *myString = new ichar[strlen("Testing") + 1];
        strcpy(myString, "Testing");
        byteStream = new ByteStream(myString);
        delete[] myString;
        CPPUNIT_ASSERT_EQUAL(String("Testing"), String(byteStream->data()));
        delete byteStream;
    }
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
