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
    String test6;
    const char *test6_1 = "file:///home/test";
    const size_t size = strlen(test6_1);
    char *test6_2 = new char[size];
    strcpy(test6_2, test6_1);
    test6 = test6_2;
    CPPUNIT_ASSERT_EQUAL(size, test6.size());
    delete[] test6_2;
}

void StringTest::testContains()
{
    {
        String str("Hello");
        CPPUNIT_ASSERT(str.contains('H'));
        CPPUNIT_ASSERT(!str.contains('h'));
        CPPUNIT_ASSERT(str.contains('e'));
        CPPUNIT_ASSERT(!str.contains('u'));
        CPPUNIT_ASSERT(str.contains('o'));
    }
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
    {
        String specialChars("file:///home/user/imágenes/spécial.png");
        CPPUNIT_ASSERT_EQUAL((size_t) 38, specialChars.size());
    }
    {
        String specialChars("šťžľčěďňřůĺ");
        CPPUNIT_ASSERT_EQUAL((size_t) 11, specialChars.size());
    }
    {
        String specialChars("абвгдеёжзийклмно");
        CPPUNIT_ASSERT_EQUAL((size_t) 16, specialChars.size());
    }
    {
        String specialChars("ЂЉЊЋЏђљњћџ");
        CPPUNIT_ASSERT_EQUAL((size_t) 10, specialChars.size());
    }
    {
        String specialChars("ščžćđ");
        CPPUNIT_ASSERT_EQUAL((size_t) 5, specialChars.size());
    }
    {
        String specialChars("ЎўЄєҐґ");
        CPPUNIT_ASSERT_EQUAL((size_t) 6, specialChars.size());
    }
    {
        String specialChars("ሰማይ አይታረስ ንጉሥ አይከሰስ።");
        CPPUNIT_ASSERT_EQUAL((size_t) 20, specialChars.size());
    }
    {
        String specialChars("⡌⠁⠧⠑ ⠼⠁⠒  ⡍⠜⠇⠑⠹⠰⠎ ⡣⠕⠌");
        CPPUNIT_ASSERT_EQUAL((size_t) 21, specialChars.size());
    }
    {
        String specialChars("ᚻᛖ ᚳᚹᚫᚦ ᚦᚫᛏ ᚻᛖ ᛒᚢᛞᛖ ᚩᚾ ᚦᚫᛗ ᛚᚪᚾᛞᛖ ᚾᚩᚱᚦᚹᛖᚪᚱᛞᚢᛗ ᚹᛁᚦ ᚦᚪ ᚹᛖᛥᚫ");
        CPPUNIT_ASSERT_EQUAL((size_t) 56, specialChars.size());
    }
    {
        String specialChars("てすと");
        CPPUNIT_ASSERT_EQUAL((size_t) 3, specialChars.size());
    }
    {
        String specialChars("ｱｲｳｴｵｶｷｸｹｺｻｼｽｾｿﾀﾁﾂﾃ");
        CPPUNIT_ASSERT_EQUAL((size_t) 19, specialChars.size());
    }
}

void StringTest::testOperators()
{
    {
        String str("This is a string");
        const int strInitialSize = str.size();
        String str2;
        str2 = str;
        CPPUNIT_ASSERT_EQUAL(str, str2);
        str += 'a';
        str2 += 'a';
        CPPUNIT_ASSERT_EQUAL(str, str2);
        str = str + 'b';
        str2 = str2 + 'b';
        CPPUNIT_ASSERT_EQUAL(str, str2);
        String add("Hello");
        str += add;
        str2 += add;
        CPPUNIT_ASSERT_EQUAL(str, str2);
        str = str + add;
        str2 = str2 + add;
        CPPUNIT_ASSERT_EQUAL(str, str2);
        const char *addStr = "Test";
        str += addStr;
        str2 += addStr;
        CPPUNIT_ASSERT_EQUAL(str, str2);
        str = str + addStr;
        str2 = str2 + addStr;
        CPPUNIT_ASSERT_EQUAL(str, str2);
        char c = 'c';
        str += c;
        str2 += c;
        CPPUNIT_ASSERT_EQUAL(str, str2);
        str = str + c;
        str2 = str2 + c;
        CPPUNIT_ASSERT_EQUAL(str, str2);
        CPPUNIT_ASSERT_EQUAL((Char) 'T', str[0]);
        CPPUNIT_ASSERT_EQUAL((Char) 'a', str[strInitialSize]);
        CPPUNIT_ASSERT_EQUAL((Char) 'b', str[strInitialSize + 1]);
        CPPUNIT_ASSERT_EQUAL((Char) 'H', str[strInitialSize + 2]);
        CPPUNIT_ASSERT_EQUAL((Char) 'T', str[strInitialSize + 12]);
    }
    {
        String str("Hello");
        String str2("How are you ?");
        CPPUNIT_ASSERT(str < str2);
    }
    {
        String str("ñ");
        String str2("z");
        CPPUNIT_ASSERT(str < str2);
    }
    {
        String str("é");
        String str2("j");
        CPPUNIT_ASSERT(str < str2);
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
