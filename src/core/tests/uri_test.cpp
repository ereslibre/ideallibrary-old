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

#include "uri_test.h"

#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <core/uri.h>
#include <core/ideal_string.h>
#include <core/application.h>

using namespace IdealCore;

CPPUNIT_TEST_SUITE_REGISTRATION(UriTest);

void UriTest::setUp()
{
}

void UriTest::tearDown()
{
}

void UriTest::testConstructor()
{
    {
        Uri test("/non/existant/path/nor/file.txt");
        CPPUNIT_ASSERT_EQUAL(String("/non/existant/path/nor/file.txt"), test.path());
        CPPUNIT_ASSERT_EQUAL(String(), test.host());
    }

    {
        Uri test("ftp://username,project:password@ftp.server.com/path/to/project/file.html");
        CPPUNIT_ASSERT(test.isValid());
        CPPUNIT_ASSERT_EQUAL(String("ftp"), test.scheme());
        CPPUNIT_ASSERT_EQUAL(String("ftp.server.com"), test.host());
        CPPUNIT_ASSERT_EQUAL(String("/path/to/project/file.html"), test.path());
        CPPUNIT_ASSERT_EQUAL(String("username,project:password"), test.userInfo());
    }

    {
        Uri test("ftp://username@ftp.host.com");
        CPPUNIT_ASSERT_EQUAL(String("ftp"), test.scheme());
        CPPUNIT_ASSERT_EQUAL(String("username"), test.userInfo());
        CPPUNIT_ASSERT_EQUAL(String("ftp.host.com"), test.host());
    }

    // Now, add some examples that can be found on the standard spec at point 1.1.2
    {
        Uri test("ftp://ftp.is.co.za/rfc/rfc1808.txt");
        CPPUNIT_ASSERT_EQUAL(String("ftp"), test.scheme());
        CPPUNIT_ASSERT_EQUAL(String("ftp.is.co.za"), test.host());
        CPPUNIT_ASSERT_EQUAL(String("/rfc/rfc1808.txt"), test.path());
    }
    {
        Uri test("http://www.ietf.org/rfc/rfc2396.txt");
        CPPUNIT_ASSERT_EQUAL(String("http"), test.scheme());
        CPPUNIT_ASSERT_EQUAL(String("www.ietf.org"), test.host());
        CPPUNIT_ASSERT_EQUAL(String("/rfc/rfc2396.txt"), test.path());
    }
    {
        Uri test("ldap://[2001:db8::7]/c=GB?objectClass?one");
        CPPUNIT_ASSERT_EQUAL(String("ldap"), test.scheme());
        CPPUNIT_ASSERT_EQUAL(String("2001:db8::7"), test.host());
    }
    {
        Uri test("mailto:John.Doe@example.com");
        CPPUNIT_ASSERT_EQUAL(String("mailto"), test.scheme());
        CPPUNIT_ASSERT_EQUAL(String("John.Doe@example.com"), test.path());
    }
    {
        Uri test("news:comp:infosystems.www.servers.unix");
        CPPUNIT_ASSERT_EQUAL(String("news"), test.scheme());
        CPPUNIT_ASSERT_EQUAL(String("comp:infosystems.www.servers.unix"), test.path());
    }
    {
        Uri test("tel:+1-816-555-1212");
        CPPUNIT_ASSERT_EQUAL(String("tel"), test.scheme());
        CPPUNIT_ASSERT_EQUAL(String("+1-816-555-1212"), test.path());
    }
    {
        Uri test("telnet://192.0.2.16:80/");
        CPPUNIT_ASSERT_EQUAL(String("telnet"), test.scheme());
        CPPUNIT_ASSERT_EQUAL(String("192.0.2.16"), test.host());
        CPPUNIT_ASSERT_EQUAL(80, test.port());
    }
    {
        Uri test("urn:oasis:names:specification:docbook:dtd:xml:4.1.2");
        CPPUNIT_ASSERT_EQUAL(String("urn"), test.scheme());
        CPPUNIT_ASSERT_EQUAL(String("oasis:names:specification:docbook:dtd:xml:4.1.2"), test.path());
    }
    // End of spec examples

    // Test for invalid URI's
    {
        Uri test("http://username:@@ftp.host.com");
        CPPUNIT_ASSERT(!test.isValid());
    }
    {
        Uri test("http://www.host.com:abc80");
        CPPUNIT_ASSERT(!test.isValid());
    }
    {
        Uri test("ftp://:password");
        CPPUNIT_ASSERT(!test.isValid());
    }
    {
        Uri test("ftp://:username:@:.com");
        CPPUNIT_ASSERT(!test.isValid());
    }

    // Other misc test cases
    {
        Uri test("file:///");
        CPPUNIT_ASSERT(test.isValid());
        CPPUNIT_ASSERT_EQUAL(String("/"), test.path());
        CPPUNIT_ASSERT_EQUAL(String(), test.host());
    }
    {
        Uri test("file:///home/user/file.txt");
        CPPUNIT_ASSERT(test.isValid());
        CPPUNIT_ASSERT_EQUAL(String("/home/user/file.txt"), test.path());
        CPPUNIT_ASSERT_EQUAL(String(), test.host());
    }
    {
        Uri test("file:///home/user/./file.png");
        CPPUNIT_ASSERT(test.isValid());
        CPPUNIT_ASSERT_EQUAL(String("/home/user/file.png"), test.path());
        CPPUNIT_ASSERT_EQUAL(String(), test.host());
    }
    {
        Uri test("file:///home/user/../file.png");
        CPPUNIT_ASSERT(test.isValid());
        CPPUNIT_ASSERT_EQUAL(String("/home/file.png"), test.path());
        CPPUNIT_ASSERT_EQUAL(String(), test.host());
    }
    {
        Uri test("file:///home/user/.././../file.png");
        CPPUNIT_ASSERT_EQUAL(String("/file.png"), test.path());
    }
    {
        Uri test("file:///home/user/..");
        CPPUNIT_ASSERT_EQUAL(String("/home"), test.path());
    }
    {
        Uri test("file:///home/user/../../");
        CPPUNIT_ASSERT_EQUAL(String("/"), test.path());
    }
    {
        Uri test("file:///home/user/.././././../");
        CPPUNIT_ASSERT_EQUAL(String("/"), test.path());
    }
    {
        Uri test("file:///home/user/../..");
        CPPUNIT_ASSERT_EQUAL(String("/"), test.path());
    }
    {
        Uri test("file:///home/user/../../../../.././../../../");
        CPPUNIT_ASSERT_EQUAL(String("/"), test.path());
    }
    {
        Uri test("file:///home/user/../../../../.././../../..");
        CPPUNIT_ASSERT_EQUAL(String("/"), test.path());
    }
    {
        Uri test("file:///home/user/folder/");
        CPPUNIT_ASSERT(test.isValid());
        CPPUNIT_ASSERT_EQUAL(String("/home/user/folder/"), test.path());
        CPPUNIT_ASSERT_EQUAL(String(), test.host());
    }
    {
        Uri test("file:///home/user/imágenes/spécial.png");
        CPPUNIT_ASSERT(test.isValid());
        CPPUNIT_ASSERT_EQUAL(String("/home/user/imágenes/spécial.png"), test.path());
        CPPUNIT_ASSERT_EQUAL(String(), test.host());
    }
    {
        Uri test("http://www.specialcháracter.com");
        CPPUNIT_ASSERT(test.isValid());
        CPPUNIT_ASSERT_EQUAL(String("www.specialcháracter.com"), test.host());
    }
    {
        Uri test("file:///home/user/imágenes/..");
        CPPUNIT_ASSERT_EQUAL(String("/home/user"), test.path());
    }
    {
        Uri test("file:///home/ﭧﭪﭷﮎ/ﮕﮒ.txt");
        CPPUNIT_ASSERT_EQUAL(String("/home/ﭧﭪﭷﮎ/ﮕﮒ.txt"), test.path());
    }
    {
        Uri test("/home/user/folder with spaces/file(2)_.txt");
        CPPUNIT_ASSERT_EQUAL(String("/home/user/folder with spaces/file(2)_.txt"), test.path());
        CPPUNIT_ASSERT_EQUAL(String("file(2)_.txt"), test.filename());
    }
}

void UriTest::testDirUp()
{
    {
        Uri uri("file:///home/user/folder1/folder2/test.txt");
        uri.dirUp();
        CPPUNIT_ASSERT_EQUAL(String("file:///home/user/folder1/folder2/"), uri.uri());
        uri.dirUp();
        CPPUNIT_ASSERT_EQUAL(String("file:///home/user/folder1/"), uri.uri());
        uri.dirUp();
        CPPUNIT_ASSERT_EQUAL(String("file:///home/user/"), uri.uri());
        uri.dirUp();
        CPPUNIT_ASSERT_EQUAL(String("file:///home/"), uri.uri());
        uri.dirUp();
        CPPUNIT_ASSERT_EQUAL(String("file:///"), uri.uri());
        uri.dirUp();
        CPPUNIT_ASSERT_EQUAL(String("file:///"), uri.uri());
    }
    {
        Uri uri("file:///home/user/folder1/folder2/");
        uri.dirUp();
        CPPUNIT_ASSERT_EQUAL(String("file:///home/user/folder1/"), uri.uri());
    }
    {
        Uri uri("/home/user/folder1/folder2/");
        uri.dirUp();
        CPPUNIT_ASSERT_EQUAL(String("/home/user/folder1/"), uri.uri());
    }
    {
        Uri uri("test.txt");
        uri.dirUp();
        CPPUNIT_ASSERT_EQUAL(String("test.txt"), uri.uri());
    }
    {
        Uri uri;
        uri.dirUp();
        CPPUNIT_ASSERT_EQUAL(String(), uri.uri());
    }
}

void UriTest::testContains()
{
    {
        Uri uri("file:///home/foo/folder");
        Uri uri2("file:///home/foo");
        CPPUNIT_ASSERT(uri.contains(uri2));
        CPPUNIT_ASSERT(!uri2.contains(uri));
    }
    {
        Uri uri("file:///home/foo/");
        Uri uri2("file:///home/foo");
        CPPUNIT_ASSERT(uri.contains(uri2));
        CPPUNIT_ASSERT(!uri2.contains(uri));
    }
    {
        Uri uri("file:///home/foo/");
        Uri uri2("file:///home/foo/");
        CPPUNIT_ASSERT(uri.contains(uri2));
        CPPUNIT_ASSERT(uri2.contains(uri));
    }
    {
        Uri uri("file:///home/foo/");
        Uri uri2("file:///home/foo/..");
        CPPUNIT_ASSERT(uri.contains(uri2));
        CPPUNIT_ASSERT(!uri2.contains(uri));
    }
    {
        Uri uri("file:///home/foo/");
        Uri uri2("file:///home/foo/../");
        CPPUNIT_ASSERT(uri.contains(uri2));
        CPPUNIT_ASSERT(!uri2.contains(uri));
    }
    {
        Uri uri("file:///home/foo/");
        Uri uri2("file:///home/foo/.");
        CPPUNIT_ASSERT(uri.contains(uri2));
        CPPUNIT_ASSERT(!uri2.contains(uri));
        CPPUNIT_ASSERT_EQUAL(String("/home/foo"), uri2.path());
        CPPUNIT_ASSERT_EQUAL(String("file:///home/foo/"), uri2.uri());
    }
    {
        Uri uri("file:///home/foo/");
        Uri uri2("file:///home/foo/./");
        CPPUNIT_ASSERT(uri.contains(uri2));
        CPPUNIT_ASSERT(uri2.contains(uri));
        CPPUNIT_ASSERT_EQUAL(String("/home/foo"), uri2.path());
        CPPUNIT_ASSERT_EQUAL(String("file:///home/foo/"), uri2.uri());
    }
    {
        Uri uri("file:///home/foo/imágenes/..");
        Uri uri2("file:///home/foo");
        CPPUNIT_ASSERT(uri.contains(uri2));
        CPPUNIT_ASSERT(uri2.contains(uri));
    }
    {
        Uri uri("file:///home/foo/imágenes", "..");
        Uri uri2("file:///home/foo");
        CPPUNIT_ASSERT(uri.contains(uri2));
        CPPUNIT_ASSERT(uri2.contains(uri));
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
