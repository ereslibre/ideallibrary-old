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
#include <core/thread.h>

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

static void runSync(Thread *job)
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
    runSync(nonExistantFile.exists(Thread::Joinable));

    File nonExistantFileWithScheme("file:///non/existant/path/nor/file.txt", s_application);
    Object::connectStatic(nonExistantFileWithScheme.existsResult, assertFalse);
    runSync(nonExistantFileWithScheme.exists(Thread::Joinable));

    File existingRoot("/", s_application);
    Object::connectStatic(existingRoot.existsResult, assertTrue);
    Object::connectStatic(existingRoot.typeResult, assertIsDirectory);
    Object::connectStatic(existingRoot.ownerUserResult, assertIsRoot);
    Object::connectStatic(existingRoot.ownerGroupResult, assertIsRoot);
    runSync(existingRoot.exists(Thread::Joinable));
    runSync(existingRoot.type(Thread::Joinable));
    runSync(existingRoot.ownerUser(Thread::Joinable));
    runSync(existingRoot.ownerGroup(Thread::Joinable));

    File existingRootWithScheme("file:///", s_application);
    Object::connectStatic(existingRootWithScheme.existsResult, assertTrue);
    Object::connectStatic(existingRootWithScheme.typeResult, assertIsDirectory);
    Object::connectStatic(existingRootWithScheme.ownerUserResult, assertIsRoot);
    Object::connectStatic(existingRootWithScheme.ownerGroupResult, assertIsRoot);
    runSync(existingRootWithScheme.exists(Thread::Joinable));
    runSync(existingRootWithScheme.type(Thread::Joinable));
    runSync(existingRootWithScheme.ownerUser(Thread::Joinable));
    runSync(existingRootWithScheme.ownerGroup(Thread::Joinable));

    File existingDir("/home/", s_application);
    Object::connectStatic(existingDir.existsResult, assertTrue);
    Object::connectStatic(existingDir.typeResult, assertIsDirectory);
    Object::connectStatic(existingDir.ownerUserResult, assertIsRoot);
    Object::connectStatic(existingDir.ownerGroupResult, assertIsRoot);
    runSync(existingDir.exists(Thread::Joinable));
    runSync(existingDir.type(Thread::Joinable));
    runSync(existingDir.ownerUser(Thread::Joinable));
    runSync(existingDir.ownerGroup(Thread::Joinable));

    File existingDirWithScheme("file:///home/", s_application);
    Object::connectStatic(existingDirWithScheme.existsResult, assertTrue);
    Object::connectStatic(existingDirWithScheme.typeResult, assertIsDirectory);
    Object::connectStatic(existingDirWithScheme.ownerUserResult, assertIsRoot);
    Object::connectStatic(existingDirWithScheme.ownerGroupResult, assertIsRoot);
    runSync(existingDirWithScheme.exists(Thread::Joinable));
    runSync(existingDirWithScheme.type(Thread::Joinable));
    runSync(existingDirWithScheme.ownerUser(Thread::Joinable));
    runSync(existingDirWithScheme.ownerGroup(Thread::Joinable));

    File nullDevice("/dev/null", s_application);
    Object::connectStatic(nullDevice.existsResult, assertTrue);
    Object::connectStatic(nullDevice.typeResult, assertIsCharacterDevice);
    Object::connectStatic(nullDevice.typeResult, assertIsNotBlockDevice);
    Object::connectStatic(nullDevice.permissionsResult, assertOthersCanWrite);
    Object::connectStatic(nullDevice.ownerUserResult, assertIsRoot);
    Object::connectStatic(nullDevice.ownerGroupResult, assertIsRoot);
    runSync(nullDevice.exists(Thread::Joinable));
    runSync(nullDevice.type(Thread::Joinable));
    runSync(nullDevice.permissions(Thread::Joinable));
    runSync(nullDevice.ownerUser(Thread::Joinable));
    runSync(nullDevice.ownerGroup(Thread::Joinable));

    File nullDeviceWithScheme("file:///dev/null", s_application);
    Object::connectStatic(nullDeviceWithScheme.existsResult, assertTrue);
    Object::connectStatic(nullDeviceWithScheme.typeResult, assertIsCharacterDevice);
    Object::connectStatic(nullDeviceWithScheme.typeResult, assertIsNotBlockDevice);
    Object::connectStatic(nullDeviceWithScheme.permissionsResult, assertOthersCanWrite);
    Object::connectStatic(nullDeviceWithScheme.ownerUserResult, assertIsRoot);
    Object::connectStatic(nullDeviceWithScheme.ownerGroupResult, assertIsRoot);
    runSync(nullDeviceWithScheme.exists(Thread::Joinable));
    runSync(nullDeviceWithScheme.type(Thread::Joinable));
    runSync(nullDeviceWithScheme.permissions(Thread::Joinable));
    runSync(nullDeviceWithScheme.ownerUser(Thread::Joinable));
    runSync(nullDeviceWithScheme.ownerGroup(Thread::Joinable));

    File remotePlace("http://www.nic.com", s_application);
    Object::connectStatic(remotePlace.existsResult, assertTrue);
    Object::connectStatic(remotePlace.error, disconnectedFromInternet);
    runSync(remotePlace.exists(Thread::Joinable));

    File nonExistantRemotePlace("http://www.nic.com/nonexistantpage", s_application);
    Object::connectStatic(nonExistantRemotePlace.existsResult, assertFalse);
    Object::connectStatic(nonExistantRemotePlace.error, disconnectedFromInternet);
    runSync(nonExistantRemotePlace.exists(Thread::Joinable));

    File oldKernel("http://www.kernel.org/pub/linux/kernel/v2.6/linux-2.6.22.1.tar.gz", s_application);
    Object::connectStatic(oldKernel.existsResult, assertTrue);
    Object::connectStatic(oldKernel.sizeResult, assertKernel22Size);
    Object::connectStatic(oldKernel.error, disconnectedFromInternet);
    runSync(oldKernel.exists(Thread::Joinable));
    runSync(oldKernel.size(Thread::Joinable));

    File oldKernelFtp("ftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.22.1.tar.gz", s_application);
    Object::connectStatic(oldKernelFtp.existsResult, assertTrue);
    Object::connectStatic(oldKernelFtp.sizeResult, assertKernel22Size);
    Object::connectStatic(oldKernelFtp.error, disconnectedFromInternet);
    runSync(oldKernelFtp.exists(Thread::Joinable));
    runSync(oldKernelFtp.size(Thread::Joinable));

    File checkError("/root/.nonexistantfile", s_application);
    Object::connectStatic(checkError.existsResult, assertFalse); // should never be called
    Object::connectStatic(checkError.error, failWithInsufficientPermissions);
    runSync(checkError.exists(Thread::Joinable));

    File checkErrorWithScheme("file:///root/.nonexistantfile", s_application);
    Object::connectStatic(checkErrorWithScheme.existsResult, assertFalse); // should never be called
    Object::connectStatic(checkErrorWithScheme.error, failWithInsufficientPermissions);
    runSync(checkErrorWithScheme.exists(Thread::Joinable));

    File checkLocalFileNotFound(s_application->getPath(Application::Home) + "/.nonexistantfile", s_application);
    Object::connectStatic(checkLocalFileNotFound.existsResult, assertFalse);
    Object::connectStatic(checkLocalFileNotFound.error, failWithFileNotFound); // should never be called
    runSync(checkLocalFileNotFound.exists(Thread::Joinable));

    File checkLocalFileNotFoundError(s_application->getPath(Application::Home) + "/.nonexistantfile", s_application);
    Object::connectStatic(checkLocalFileNotFoundError.sizeResult, assertKernel22Size); // should never be called
    Object::connectStatic(checkLocalFileNotFoundError.error, failWithFileNotFound);
    runSync(checkLocalFileNotFoundError.size(Thread::Joinable));

    File checkRemoteFileNotFound("ftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.22.1.tar.gzzzz", s_application);
    Object::connectStatic(checkRemoteFileNotFound.existsResult, assertFalse);
    Object::connectStatic(checkRemoteFileNotFound.error, disconnectedFromInternet);
    runSync(checkRemoteFileNotFound.exists(Thread::Joinable));

    File checkRemoteFileNotFoundError("ftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.22.1.tar.gzzzz", s_application);
    Object::connectStatic(checkRemoteFileNotFoundError.error, disconnectedFromInternetOrFileNotFound);
    runSync(checkRemoteFileNotFoundError.size(Thread::Joinable));

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
