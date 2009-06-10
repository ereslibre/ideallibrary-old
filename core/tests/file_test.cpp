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
    runSync(nonExistantFile.exists(Concurrent::Joinable));

    File nonExistantFileWithScheme("file:///non/existant/path/nor/file.txt", s_application);
    Object::connectStatic(nonExistantFileWithScheme.existsResult, assertFalse);
    runSync(nonExistantFileWithScheme.exists(Concurrent::Joinable));

    File existingRoot("/", s_application);
    Object::connectStatic(existingRoot.existsResult, assertTrue);
    Object::connectStatic(existingRoot.typeResult, assertIsDirectory);
    Object::connectStatic(existingRoot.ownerUserResult, assertIsRoot);
    Object::connectStatic(existingRoot.ownerGroupResult, assertIsRoot);
    runSync(existingRoot.exists(Concurrent::Joinable));
    runSync(existingRoot.type(Concurrent::Joinable));
    runSync(existingRoot.ownerUser(Concurrent::Joinable));
    runSync(existingRoot.ownerGroup(Concurrent::Joinable));

    File existingRootWithScheme("file:///", s_application);
    Object::connectStatic(existingRootWithScheme.existsResult, assertTrue);
    Object::connectStatic(existingRootWithScheme.typeResult, assertIsDirectory);
    Object::connectStatic(existingRootWithScheme.ownerUserResult, assertIsRoot);
    Object::connectStatic(existingRootWithScheme.ownerGroupResult, assertIsRoot);
    runSync(existingRootWithScheme.exists(Concurrent::Joinable));
    runSync(existingRootWithScheme.type(Concurrent::Joinable));
    runSync(existingRootWithScheme.ownerUser(Concurrent::Joinable));
    runSync(existingRootWithScheme.ownerGroup(Concurrent::Joinable));

    File existingDir("/home/", s_application);
    Object::connectStatic(existingDir.existsResult, assertTrue);
    Object::connectStatic(existingDir.typeResult, assertIsDirectory);
    Object::connectStatic(existingDir.ownerUserResult, assertIsRoot);
    Object::connectStatic(existingDir.ownerGroupResult, assertIsRoot);
    runSync(existingDir.exists(Concurrent::Joinable));
    runSync(existingDir.type(Concurrent::Joinable));
    runSync(existingDir.ownerUser(Concurrent::Joinable));
    runSync(existingDir.ownerGroup(Concurrent::Joinable));

    File existingDirWithScheme("file:///home/", s_application);
    Object::connectStatic(existingDirWithScheme.existsResult, assertTrue);
    Object::connectStatic(existingDirWithScheme.typeResult, assertIsDirectory);
    Object::connectStatic(existingDirWithScheme.ownerUserResult, assertIsRoot);
    Object::connectStatic(existingDirWithScheme.ownerGroupResult, assertIsRoot);
    runSync(existingDirWithScheme.exists(Concurrent::Joinable));
    runSync(existingDirWithScheme.type(Concurrent::Joinable));
    runSync(existingDirWithScheme.ownerUser(Concurrent::Joinable));
    runSync(existingDirWithScheme.ownerGroup(Concurrent::Joinable));

    File nullDevice("/dev/null", s_application);
    Object::connectStatic(nullDevice.existsResult, assertTrue);
    Object::connectStatic(nullDevice.typeResult, assertIsCharacterDevice);
    Object::connectStatic(nullDevice.typeResult, assertIsNotBlockDevice);
    Object::connectStatic(nullDevice.permissionsResult, assertOthersCanWrite);
    Object::connectStatic(nullDevice.ownerUserResult, assertIsRoot);
    Object::connectStatic(nullDevice.ownerGroupResult, assertIsRoot);
    runSync(nullDevice.exists(Concurrent::Joinable));
    runSync(nullDevice.type(Concurrent::Joinable));
    runSync(nullDevice.permissions(Concurrent::Joinable));
    runSync(nullDevice.ownerUser(Concurrent::Joinable));
    runSync(nullDevice.ownerGroup(Concurrent::Joinable));

    File nullDeviceWithScheme("file:///dev/null", s_application);
    Object::connectStatic(nullDeviceWithScheme.existsResult, assertTrue);
    Object::connectStatic(nullDeviceWithScheme.typeResult, assertIsCharacterDevice);
    Object::connectStatic(nullDeviceWithScheme.typeResult, assertIsNotBlockDevice);
    Object::connectStatic(nullDeviceWithScheme.permissionsResult, assertOthersCanWrite);
    Object::connectStatic(nullDeviceWithScheme.ownerUserResult, assertIsRoot);
    Object::connectStatic(nullDeviceWithScheme.ownerGroupResult, assertIsRoot);
    runSync(nullDeviceWithScheme.exists(Concurrent::Joinable));
    runSync(nullDeviceWithScheme.type(Concurrent::Joinable));
    runSync(nullDeviceWithScheme.permissions(Concurrent::Joinable));
    runSync(nullDeviceWithScheme.ownerUser(Concurrent::Joinable));
    runSync(nullDeviceWithScheme.ownerGroup(Concurrent::Joinable));

    File remotePlace("http://www.nic.com", s_application);
    Object::connectStatic(remotePlace.existsResult, assertTrue);
    Object::connectStatic(remotePlace.error, disconnectedFromInternet);
    runSync(remotePlace.exists(Concurrent::Joinable));

    File nonExistantRemotePlace("http://www.nic.com/nonexistantpage", s_application);
    Object::connectStatic(nonExistantRemotePlace.existsResult, assertFalse);
    Object::connectStatic(nonExistantRemotePlace.error, disconnectedFromInternet);
    runSync(nonExistantRemotePlace.exists(Concurrent::Joinable));

    File oldKernel("http://www.kernel.org/pub/linux/kernel/v2.6/linux-2.6.22.1.tar.gz", s_application);
    Object::connectStatic(oldKernel.existsResult, assertTrue);
    Object::connectStatic(oldKernel.sizeResult, assertKernel22Size);
    Object::connectStatic(oldKernel.error, disconnectedFromInternet);
    runSync(oldKernel.exists(Concurrent::Joinable));
    runSync(oldKernel.size(Concurrent::Joinable));

    File oldKernelFtp("ftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.22.1.tar.gz", s_application);
    Object::connectStatic(oldKernelFtp.existsResult, assertTrue);
    Object::connectStatic(oldKernelFtp.sizeResult, assertKernel22Size);
    Object::connectStatic(oldKernelFtp.error, disconnectedFromInternet);
    runSync(oldKernelFtp.exists(Concurrent::Joinable));
    runSync(oldKernelFtp.size(Concurrent::Joinable));

    File checkError("/root/.nonexistantfile", s_application);
    Object::connectStatic(checkError.existsResult, assertFalse); // should never be called
    Object::connectStatic(checkError.error, failWithInsufficientPermissions);
    runSync(checkError.exists(Concurrent::Joinable));

    File checkErrorWithScheme("file:///root/.nonexistantfile", s_application);
    Object::connectStatic(checkErrorWithScheme.existsResult, assertFalse); // should never be called
    Object::connectStatic(checkErrorWithScheme.error, failWithInsufficientPermissions);
    runSync(checkErrorWithScheme.exists(Concurrent::Joinable));

    File checkLocalFileNotFound(s_application->getPath(Application::Home) + "/.nonexistantfile", s_application);
    Object::connectStatic(checkLocalFileNotFound.existsResult, assertFalse);
    Object::connectStatic(checkLocalFileNotFound.error, failWithFileNotFound); // should never be called
    runSync(checkLocalFileNotFound.exists(Concurrent::Joinable));

    File checkLocalFileNotFoundError(s_application->getPath(Application::Home) + "/.nonexistantfile", s_application);
    Object::connectStatic(checkLocalFileNotFoundError.sizeResult, assertKernel22Size); // should never be called
    Object::connectStatic(checkLocalFileNotFoundError.error, failWithFileNotFound);
    runSync(checkLocalFileNotFoundError.size(Concurrent::Joinable));

    File checkRemoteFileNotFound("ftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.22.1.tar.gzzzz", s_application);
    Object::connectStatic(checkRemoteFileNotFound.existsResult, assertFalse);
    Object::connectStatic(checkRemoteFileNotFound.error, disconnectedFromInternet);
    runSync(checkRemoteFileNotFound.exists(Concurrent::Joinable));

    File checkRemoteFileNotFoundError("ftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.22.1.tar.gzzzz", s_application);
    Object::connectStatic(checkRemoteFileNotFoundError.error, disconnectedFromInternetOrFileNotFound);
    runSync(checkRemoteFileNotFoundError.size(Concurrent::Joinable));

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
