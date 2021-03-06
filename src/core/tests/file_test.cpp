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
static iint32 numCalls = 0;
static bool disconnectedWarned = false;

static void assertExists(const ProtocolHandler::StatResult &statResult)
{
    CPPUNIT_ASSERT(statResult.errorCode != ProtocolHandler::FileNotFound);
    ++numCalls;
}

static void assertFileDoesNotExist(const ProtocolHandler::StatResult &statResult)
{
    CPPUNIT_ASSERT(statResult.errorCode == ProtocolHandler::FileNotFound);
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
    CPPUNIT_ASSERT_EQUAL((size_t) 56919686, statResult.size);
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
    nonExistantFile.statResult.connectStatic(assertFileDoesNotExist);
    runSync(nonExistantFile.stat(Thread::Joinable));

    File nonExistantFileWithScheme("file:///non/existant/path/nor/file.txt", s_application);
    nonExistantFileWithScheme.statResult.connectStatic(assertFileDoesNotExist);
    runSync(nonExistantFileWithScheme.stat(Thread::Joinable));

    File existingRoot("/", s_application);
    existingRoot.statResult.connectStatic(assertExists);
    existingRoot.statResult.connectStatic(assertIsDirectory);
    existingRoot.statResult.connectStatic(assertIsRoot);
    existingRoot.statResult.connectStatic(assertIsRoot);
    runSync(existingRoot.stat(Thread::Joinable));

    File existingRootWithScheme("file:///", s_application);
    existingRootWithScheme.statResult.connectStatic(assertExists);
    existingRootWithScheme.statResult.connectStatic(assertIsDirectory);
    existingRootWithScheme.statResult.connectStatic(assertIsRoot);
    existingRootWithScheme.statResult.connectStatic(assertIsRoot);
    runSync(existingRootWithScheme.stat(Thread::Joinable));

    File existingDir("/home/", s_application);
    existingDir.statResult.connectStatic(assertExists);
    existingDir.statResult.connectStatic(assertIsDirectory);
    existingDir.statResult.connectStatic(assertIsRoot);
    existingDir.statResult.connectStatic(assertIsRoot);
    runSync(existingDir.stat(Thread::Joinable));

    File existingDirWithScheme("file:///home/", s_application);
    existingDirWithScheme.statResult.connectStatic(assertExists);
    existingDirWithScheme.statResult.connectStatic(assertIsDirectory);
    existingDirWithScheme.statResult.connectStatic(assertIsRoot);
    existingDirWithScheme.statResult.connectStatic(assertIsRoot);
    runSync(existingDirWithScheme.stat(Thread::Joinable));

    File nullDevice("/dev/null", s_application);
    nullDevice.statResult.connectStatic(assertExists);
    nullDevice.statResult.connectStatic(assertIsCharacterDevice);
    nullDevice.statResult.connectStatic(assertIsNotBlockDevice);
    nullDevice.statResult.connectStatic(assertOthersCanWrite);
    nullDevice.statResult.connectStatic(assertIsRoot);
    nullDevice.statResult.connectStatic(assertIsRoot);
    runSync(nullDevice.stat(Thread::Joinable));

    File nullDeviceWithScheme("file:///dev/null", s_application);
    nullDeviceWithScheme.statResult.connectStatic(assertExists);
    nullDeviceWithScheme.statResult.connectStatic(assertIsCharacterDevice);
    nullDeviceWithScheme.statResult.connectStatic(assertIsNotBlockDevice);
    nullDeviceWithScheme.statResult.connectStatic(assertOthersCanWrite);
    nullDeviceWithScheme.statResult.connectStatic(assertIsRoot);
    nullDeviceWithScheme.statResult.connectStatic(assertIsRoot);
    runSync(nullDeviceWithScheme.stat(Thread::Joinable));

    File remotePlace("http://www.nic.com", s_application);
    remotePlace.statResult.connectStatic(assertExists);
    remotePlace.error.connectStatic(disconnectedFromInternet);
    runSync(remotePlace.stat(Thread::Joinable));

    File nonExistantRemotePlace("http://www.nic.com/nonexistantpage", s_application);
    nonExistantRemotePlace.statResult.connectStatic(assertFileDoesNotExist);
    nonExistantRemotePlace.error.connectStatic(disconnectedFromInternet);
    runSync(nonExistantRemotePlace.stat(Thread::Joinable));

    File oldKernel("http://www.kernel.org/pub/linux/kernel/v2.6/linux-2.6.22.1.tar.gz", s_application);
    oldKernel.statResult.connectStatic(assertExists);
    oldKernel.statResult.connectStatic(assertKernel22Size);
    oldKernel.error.connectStatic(disconnectedFromInternet);
    runSync(oldKernel.stat(Thread::Joinable));

    File oldKernelFtp("ftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.22.1.tar.gz", s_application);
    oldKernelFtp.statResult.connectStatic(assertExists);
    oldKernelFtp.statResult.connectStatic(assertKernel22Size);
    oldKernelFtp.error.connectStatic(disconnectedFromInternet);
    runSync(oldKernelFtp.stat(Thread::Joinable));

    File checkError("/root/.nonexistantfile", s_application);
    checkError.statResult.connectStatic(assertFileDoesNotExist); // should never be called
    checkError.error.connectStatic(failWithInsufficientPermissions);
    runSync(checkError.stat(Thread::Joinable));

    File checkErrorWithScheme("file:///root/.nonexistantfile", s_application);
    checkErrorWithScheme.statResult.connectStatic(assertFileDoesNotExist); // should never be called
    checkErrorWithScheme.error.connectStatic(failWithInsufficientPermissions);
    runSync(checkErrorWithScheme.stat(Thread::Joinable));

    File checkLocalFileNotFound(s_application->getPath(Application::Home) + "/.nonexistantfile", s_application);
    checkLocalFileNotFound.statResult.connectStatic(assertFileDoesNotExist);
    checkLocalFileNotFound.error.connectStatic(failWithFileNotFound); // should never be called
    runSync(checkLocalFileNotFound.stat(Thread::Joinable));

    File checkLocalFileNotFoundError(s_application->getPath(Application::Home) + "/.nonexistantfile", s_application);
    checkLocalFileNotFoundError.statResult.connectStatic(assertKernel22Size); // should never be called
    checkLocalFileNotFoundError.error.connectStatic(failWithFileNotFound);
    runSync(checkLocalFileNotFoundError.stat(Thread::Joinable));

    File checkRemoteFileNotFound("ftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.22.1.tar.gzzzz", s_application);
    checkRemoteFileNotFound.statResult.connectStatic(assertFileDoesNotExist);
    checkRemoteFileNotFound.error.connectStatic(disconnectedFromInternet);
    runSync(checkRemoteFileNotFound.stat(Thread::Joinable));

    File checkRemoteFileNotFoundError("ftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.22.1.tar.gzzzz", s_application);
    checkRemoteFileNotFoundError.error.connectStatic(disconnectedFromInternetOrFileNotFound);
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
