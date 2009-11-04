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

static void assertExists(const ProtocolHandler::StatResult &statResult)
{
    CPPUNIT_ASSERT(statResult.errorCode != ProtocolHandler::FileNotFound);
    ++numCalls;
}

static void assertFileDoesNotExist(const ProtocolHandler::StatResult &statResult)
{
    CPPUNIT_ASSERT(!statResult.errorCode != ProtocolHandler::FileNotFound);
    ++numCalls;
}

static void assertIsDirectory(const ProtocolHandler::StatResult &statResult)
{
    CPPUNIT_ASSERT(statResult.type & ProtocolHandler::Directory);
    ++numCalls;
}

static void assertIsRoot(const ProtocolHandler::StatResult &statResult)
{
    CPPUNIT_ASSERT_EQUAL(String("root"), statResult.ownerUser);
    ++numCalls;
}

static void assertIsCharacterDevice(const ProtocolHandler::StatResult &statResult)
{
    CPPUNIT_ASSERT(statResult.type & ProtocolHandler::CharacterDevice);
    CPPUNIT_ASSERT(statResult.type & ProtocolHandler::Device);
    ++numCalls;
}

static void assertIsNotBlockDevice(const ProtocolHandler::StatResult &statResult)
{
    CPPUNIT_ASSERT(!(statResult.type & ProtocolHandler::BlockDevice));
    ++numCalls;
}

static void assertOthersCanWrite(const ProtocolHandler::StatResult &statResult)
{
    CPPUNIT_ASSERT(statResult.permissions & ProtocolHandler::OthersCanWrite);
    ++numCalls;
}

static void assertKernel22Size(const ProtocolHandler::StatResult &statResult)
{
    CPPUNIT_ASSERT_EQUAL((iuint64) 56919686, statResult.size);
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
    Object::connectStatic(nonExistantFile.statResult, assertFileDoesNotExist);
    runSync(nonExistantFile.stat(Thread::Joinable));

    File nonExistantFileWithScheme("file:///non/existant/path/nor/file.txt", s_application);
    Object::connectStatic(nonExistantFileWithScheme.statResult, assertFileDoesNotExist);
    runSync(nonExistantFileWithScheme.stat(Thread::Joinable));

    File existingRoot("/", s_application);
    Object::connectStatic(existingRoot.statResult, assertExists);
    Object::connectStatic(existingRoot.statResult, assertIsDirectory);
    Object::connectStatic(existingRoot.statResult, assertIsRoot);
    Object::connectStatic(existingRoot.statResult, assertIsRoot);
    runSync(existingRoot.stat(Thread::Joinable));

    File existingRootWithScheme("file:///", s_application);
    Object::connectStatic(existingRootWithScheme.statResult, assertExists);
    Object::connectStatic(existingRootWithScheme.statResult, assertIsDirectory);
    Object::connectStatic(existingRootWithScheme.statResult, assertIsRoot);
    Object::connectStatic(existingRootWithScheme.statResult, assertIsRoot);
    runSync(existingRootWithScheme.stat(Thread::Joinable));

    File existingDir("/home/", s_application);
    Object::connectStatic(existingDir.statResult, assertExists);
    Object::connectStatic(existingDir.statResult, assertIsDirectory);
    Object::connectStatic(existingDir.statResult, assertIsRoot);
    Object::connectStatic(existingDir.statResult, assertIsRoot);
    runSync(existingDir.stat(Thread::Joinable));

    File existingDirWithScheme("file:///home/", s_application);
    Object::connectStatic(existingDirWithScheme.statResult, assertExists);
    Object::connectStatic(existingDirWithScheme.statResult, assertIsDirectory);
    Object::connectStatic(existingDirWithScheme.statResult, assertIsRoot);
    Object::connectStatic(existingDirWithScheme.statResult, assertIsRoot);
    runSync(existingDirWithScheme.stat(Thread::Joinable));

    File nullDevice("/dev/null", s_application);
    Object::connectStatic(nullDevice.statResult, assertExists);
    Object::connectStatic(nullDevice.statResult, assertIsCharacterDevice);
    Object::connectStatic(nullDevice.statResult, assertIsNotBlockDevice);
    Object::connectStatic(nullDevice.statResult, assertOthersCanWrite);
    Object::connectStatic(nullDevice.statResult, assertIsRoot);
    Object::connectStatic(nullDevice.statResult, assertIsRoot);
    runSync(nullDevice.stat(Thread::Joinable));

    File nullDeviceWithScheme("file:///dev/null", s_application);
    Object::connectStatic(nullDeviceWithScheme.statResult, assertExists);
    Object::connectStatic(nullDeviceWithScheme.statResult, assertIsCharacterDevice);
    Object::connectStatic(nullDeviceWithScheme.statResult, assertIsNotBlockDevice);
    Object::connectStatic(nullDeviceWithScheme.statResult, assertOthersCanWrite);
    Object::connectStatic(nullDeviceWithScheme.statResult, assertIsRoot);
    Object::connectStatic(nullDeviceWithScheme.statResult, assertIsRoot);
    runSync(nullDeviceWithScheme.stat(Thread::Joinable));

    File remotePlace("http://www.nic.com", s_application);
    Object::connectStatic(remotePlace.statResult, assertExists);
    Object::connectStatic(remotePlace.error, disconnectedFromInternet);
    runSync(remotePlace.stat(Thread::Joinable));

    File nonExistantRemotePlace("http://www.nic.com/nonexistantpage", s_application);
    Object::connectStatic(nonExistantRemotePlace.statResult, assertFileDoesNotExist);
    Object::connectStatic(nonExistantRemotePlace.error, disconnectedFromInternet);
    runSync(nonExistantRemotePlace.stat(Thread::Joinable));

    File oldKernel("http://www.kernel.org/pub/linux/kernel/v2.6/linux-2.6.22.1.tar.gz", s_application);
    Object::connectStatic(oldKernel.statResult, assertExists);
    Object::connectStatic(oldKernel.statResult, assertKernel22Size);
    Object::connectStatic(oldKernel.error, disconnectedFromInternet);
    runSync(oldKernel.stat(Thread::Joinable));

    File oldKernelFtp("ftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.22.1.tar.gz", s_application);
    Object::connectStatic(oldKernelFtp.statResult, assertExists);
    Object::connectStatic(oldKernelFtp.statResult, assertKernel22Size);
    Object::connectStatic(oldKernelFtp.error, disconnectedFromInternet);
    runSync(oldKernelFtp.stat(Thread::Joinable));

    File checkError("/root/.nonexistantfile", s_application);
    Object::connectStatic(checkError.statResult, assertFileDoesNotExist); // should never be called
    Object::connectStatic(checkError.error, failWithInsufficientPermissions);
    runSync(checkError.stat(Thread::Joinable));

    File checkErrorWithScheme("file:///root/.nonexistantfile", s_application);
    Object::connectStatic(checkErrorWithScheme.statResult, assertFileDoesNotExist); // should never be called
    Object::connectStatic(checkErrorWithScheme.error, failWithInsufficientPermissions);
    runSync(checkErrorWithScheme.stat(Thread::Joinable));

    File checkLocalFileNotFound(s_application->getPath(Application::Home) + "/.nonexistantfile", s_application);
    Object::connectStatic(checkLocalFileNotFound.statResult, assertFileDoesNotExist);
    Object::connectStatic(checkLocalFileNotFound.error, failWithFileNotFound); // should never be called
    runSync(checkLocalFileNotFound.stat(Thread::Joinable));

    File checkLocalFileNotFoundError(s_application->getPath(Application::Home) + "/.nonexistantfile", s_application);
    Object::connectStatic(checkLocalFileNotFoundError.statResult, assertKernel22Size); // should never be called
    Object::connectStatic(checkLocalFileNotFoundError.error, failWithFileNotFound);
    runSync(checkLocalFileNotFoundError.stat(Thread::Joinable));

    File checkRemoteFileNotFound("ftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.22.1.tar.gzzzz", s_application);
    Object::connectStatic(checkRemoteFileNotFound.statResult, assertFileDoesNotExist);
    Object::connectStatic(checkRemoteFileNotFound.error, disconnectedFromInternet);
    runSync(checkRemoteFileNotFound.stat(Thread::Joinable));

    File checkRemoteFileNotFoundError("ftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.22.1.tar.gzzzz", s_application);
    Object::connectStatic(checkRemoteFileNotFoundError.error, disconnectedFromInternetOrFileNotFound);
    runSync(checkRemoteFileNotFoundError.stat(Thread::Joinable));

    CPPUNIT_ASSERT_EQUAL(35, numCalls);
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
