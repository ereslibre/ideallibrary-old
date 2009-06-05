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

#include "file_test.h"

#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <core/file.h>
#include <core/application.h>
#include <core/concurrent.h>

using namespace IdealCore;

CPPUNIT_TEST_SUITE_REGISTRATION(FileTest);

static Application *s_application = 0;
static int numCalls = 0;
static bool disconnectedWarned = false;

static void assertTrue(bool result)
{
    CPPUNIT_ASSERT(result);
    ++numCalls;
}

static void assertFalse(bool result)
{
    CPPUNIT_ASSERT(!result);
    ++numCalls;
}

static void assertIsDirectory(File::Type type)
{
    CPPUNIT_ASSERT(type & File::Directory);
    ++numCalls;
}

static void assertIsRoot(const String &string)
{
    CPPUNIT_ASSERT_EQUAL(String("root"), string);
    ++numCalls;
}

static void assertIsCharacterDevice(File::Type type)
{
    CPPUNIT_ASSERT(type & File::CharacterDevice);
    CPPUNIT_ASSERT(type & File::Device);
    ++numCalls;
}

static void assertIsNotBlockDevice(File::Type type)
{
    CPPUNIT_ASSERT(!(type & File::BlockDevice));
    ++numCalls;
}

static void assertOthersCanWrite(File::Permissions permissions)
{
    CPPUNIT_ASSERT(permissions & File::OthersCanWrite);
    ++numCalls;
}

static void assertKernel22Size(double size)
{
    CPPUNIT_ASSERT_EQUAL((double) 56919686, size);
    ++numCalls;
}

static void failWithInsufficientPermissions(ProtocolHandler::ErrorCode errorCode)
{
    CPPUNIT_ASSERT_EQUAL(ProtocolHandler::InsufficientPermissions, errorCode);
    ++numCalls;
}

static void failWithFileNotFound(ProtocolHandler::ErrorCode errorCode)
{
    CPPUNIT_ASSERT_EQUAL(ProtocolHandler::FileNotFound, errorCode);
    ++numCalls;
}

static void disconnectedFromInternet(ProtocolHandler::ErrorCode errorCode)
{
    CPPUNIT_ASSERT(errorCode == ProtocolHandler::CouldNotResolveHost ||
                   errorCode == ProtocolHandler::CouldNotConnect);
    if (!disconnectedWarned) {
        disconnectedWarned = true;
        IDEAL_SDEBUG("*** You have no Internet connection. Please retry test when you have restored it");
    }
    ++numCalls;
}

static void disconnectedFromInternetOrFileNotFound(ProtocolHandler::ErrorCode errorCode)
{
    CPPUNIT_ASSERT(errorCode == ProtocolHandler::CouldNotResolveHost ||
                   errorCode == ProtocolHandler::CouldNotConnect ||
                   errorCode == ProtocolHandler::FileNotFound);
    if (!disconnectedWarned && (errorCode == ProtocolHandler::CouldNotResolveHost ||
                                errorCode == ProtocolHandler::CouldNotConnect)) {
        disconnectedWarned = true;
        IDEAL_SDEBUG("*** You have no Internet connection. Please retry test when you have restored it");
    }
    ++numCalls;
}

static void runSync(Concurrent *job)
{
    job->exec();
    job->join();
}

void FileTest::setUp()
{
}

void FileTest::tearDown()
{
}

void FileTest::testConstructor()
{
    File nonExistantFile("/non/existant/path/nor/file.txt", s_application);
    Object::connectStatic(nonExistantFile.existsResult, assertFalse);
    runSync(nonExistantFile.exists());

    File nonExistantFileWithScheme("file:///non/existant/path/nor/file.txt", s_application);
    Object::connectStatic(nonExistantFileWithScheme.existsResult, assertFalse);
    runSync(nonExistantFileWithScheme.exists());

    File existingRoot("/", s_application);
    Object::connectStatic(existingRoot.existsResult, assertTrue);
    Object::connectStatic(existingRoot.typeResult, assertIsDirectory);
    Object::connectStatic(existingRoot.ownerUserResult, assertIsRoot);
    Object::connectStatic(existingRoot.ownerGroupResult, assertIsRoot);
    runSync(existingRoot.exists());
    runSync(existingRoot.type());
    runSync(existingRoot.ownerUser());
    runSync(existingRoot.ownerGroup());

    File existingRootWithScheme("file:///", s_application);
    Object::connectStatic(existingRootWithScheme.existsResult, assertTrue);
    Object::connectStatic(existingRootWithScheme.typeResult, assertIsDirectory);
    Object::connectStatic(existingRootWithScheme.ownerUserResult, assertIsRoot);
    Object::connectStatic(existingRootWithScheme.ownerGroupResult, assertIsRoot);
    runSync(existingRootWithScheme.exists());
    runSync(existingRootWithScheme.type());
    runSync(existingRootWithScheme.ownerUser());
    runSync(existingRootWithScheme.ownerGroup());

    File existingDir("/home/", s_application);
    Object::connectStatic(existingDir.existsResult, assertTrue);
    Object::connectStatic(existingDir.typeResult, assertIsDirectory);
    Object::connectStatic(existingDir.ownerUserResult, assertIsRoot);
    Object::connectStatic(existingDir.ownerGroupResult, assertIsRoot);
    runSync(existingDir.exists());
    runSync(existingDir.type());
    runSync(existingDir.ownerUser());
    runSync(existingDir.ownerGroup());

    File existingDirWithScheme("file:///home/", s_application);
    Object::connectStatic(existingDirWithScheme.existsResult, assertTrue);
    Object::connectStatic(existingDirWithScheme.typeResult, assertIsDirectory);
    Object::connectStatic(existingDirWithScheme.ownerUserResult, assertIsRoot);
    Object::connectStatic(existingDirWithScheme.ownerGroupResult, assertIsRoot);
    runSync(existingDirWithScheme.exists());
    runSync(existingDirWithScheme.type());
    runSync(existingDirWithScheme.ownerUser());
    runSync(existingDirWithScheme.ownerGroup());

    File nullDevice("/dev/null", s_application);
    Object::connectStatic(nullDevice.existsResult, assertTrue);
    Object::connectStatic(nullDevice.typeResult, assertIsCharacterDevice);
    Object::connectStatic(nullDevice.typeResult, assertIsNotBlockDevice);
    Object::connectStatic(nullDevice.permissionsResult, assertOthersCanWrite);
    Object::connectStatic(nullDevice.ownerUserResult, assertIsRoot);
    Object::connectStatic(nullDevice.ownerGroupResult, assertIsRoot);
    runSync(nullDevice.exists());
    runSync(nullDevice.type());
    runSync(nullDevice.permissions());
    runSync(nullDevice.ownerUser());
    runSync(nullDevice.ownerGroup());

    File nullDeviceWithScheme("file:///dev/null", s_application);
    Object::connectStatic(nullDeviceWithScheme.existsResult, assertTrue);
    Object::connectStatic(nullDeviceWithScheme.typeResult, assertIsCharacterDevice);
    Object::connectStatic(nullDeviceWithScheme.typeResult, assertIsNotBlockDevice);
    Object::connectStatic(nullDeviceWithScheme.permissionsResult, assertOthersCanWrite);
    Object::connectStatic(nullDeviceWithScheme.ownerUserResult, assertIsRoot);
    Object::connectStatic(nullDeviceWithScheme.ownerGroupResult, assertIsRoot);
    runSync(nullDeviceWithScheme.exists());
    runSync(nullDeviceWithScheme.type());
    runSync(nullDeviceWithScheme.permissions());
    runSync(nullDeviceWithScheme.ownerUser());
    runSync(nullDeviceWithScheme.ownerGroup());

    File remotePlace("http://www.nic.com", s_application);
    Object::connectStatic(remotePlace.existsResult, assertTrue);
    Object::connectStatic(remotePlace.error, disconnectedFromInternet);
    runSync(remotePlace.exists());

    File nonExistantRemotePlace("http://www.nic.com/nonexistantpage", s_application);
    Object::connectStatic(nonExistantRemotePlace.existsResult, assertFalse);
    Object::connectStatic(nonExistantRemotePlace.error, disconnectedFromInternet);
    runSync(nonExistantRemotePlace.exists());

    File oldKernel("http://www.kernel.org/pub/linux/kernel/v2.6/linux-2.6.22.1.tar.gz", s_application);
    Object::connectStatic(oldKernel.existsResult, assertTrue);
    Object::connectStatic(oldKernel.sizeResult, assertKernel22Size);
    Object::connectStatic(oldKernel.error, disconnectedFromInternet);
    runSync(oldKernel.exists());
    runSync(oldKernel.size());

    File oldKernelFtp("ftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.22.1.tar.gz", s_application);
    Object::connectStatic(oldKernelFtp.existsResult, assertTrue);
    Object::connectStatic(oldKernelFtp.sizeResult, assertKernel22Size);
    Object::connectStatic(oldKernelFtp.error, disconnectedFromInternet);
    runSync(oldKernelFtp.exists());
    runSync(oldKernelFtp.size());

    File checkError("/root/.nonexistantfile", s_application);
    Object::connectStatic(checkError.existsResult, assertFalse); // should never be called
    Object::connectStatic(checkError.error, failWithInsufficientPermissions);
    runSync(checkError.exists());

    File checkErrorWithScheme("file:///root/.nonexistantfile", s_application);
    Object::connectStatic(checkErrorWithScheme.existsResult, assertFalse); // should never be called
    Object::connectStatic(checkErrorWithScheme.error, failWithInsufficientPermissions);
    runSync(checkErrorWithScheme.exists());

    File checkLocalFileNotFound(s_application->getPath(Application::Home) + "/.nonexistantfile", s_application);
    Object::connectStatic(checkLocalFileNotFound.existsResult, assertFalse);
    Object::connectStatic(checkLocalFileNotFound.error, failWithFileNotFound); // should never be called
    runSync(checkLocalFileNotFound.exists());

    File checkLocalFileNotFoundError(s_application->getPath(Application::Home) + "/.nonexistantfile", s_application);
    Object::connectStatic(checkLocalFileNotFoundError.sizeResult, assertKernel22Size); // should never be called
    Object::connectStatic(checkLocalFileNotFoundError.error, failWithFileNotFound);
    runSync(checkLocalFileNotFoundError.size());

    File checkRemoteFileNotFound("ftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.22.1.tar.gzzzz", s_application);
    Object::connectStatic(checkRemoteFileNotFound.existsResult, assertFalse);
    Object::connectStatic(checkRemoteFileNotFound.error, disconnectedFromInternet);
    runSync(checkRemoteFileNotFound.exists());

    File checkRemoteFileNotFoundError("ftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.22.1.tar.gzzzz", s_application);
    Object::connectStatic(checkRemoteFileNotFoundError.error, disconnectedFromInternetOrFileNotFound);
    runSync(checkRemoteFileNotFoundError.size());

    CPPUNIT_ASSERT_EQUAL(42, numCalls);
}

int main(int argc, char **argv)
{
    Application app(argc, argv);
    s_application = &app;

    CppUnit::Test *suite = CppUnit::TestFactoryRegistry::getRegistry().makeTest();

    CppUnit::TextUi::TestRunner runner;
    runner.addTest(suite);

    runner.setOutputter(new CppUnit::CompilerOutputter(&runner.result(), std::cerr));
    bool wasSuccessful = runner.run();

    return wasSuccessful ? 0 : 1;
}
