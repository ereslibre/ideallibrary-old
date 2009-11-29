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

#include <core/application.h>
#include <core/thread.h>
#include <core/file.h>
#include <core/uri.h>

using namespace IdealCore;

static void fileSize(const ProtocolHandler::StatResult &statResult)
{
    IDEAL_SDEBUG("\tSize:\t" << (statResult.size / (1024 * 1024)) << " MiB");
}

static void error(Object *file, ProtocolHandler::ErrorCode errorCode)
{
    IDEAL_SDEBUG("\tError (" <<  errorCode << ") reading: " << static_cast<File*>(file)->uri().uri());
}

class MyApplication
    : public Application
{
public:
    MyApplication(iint32 argc, ichar **argv)
        : Application(argc, argv)
        , numReceived(0)
    {
    }

    void fileSize(Object *file, const ProtocolHandler::StatResult &statResult)
    {
        IDEAL_SDEBUG("*** File:\t" << static_cast<File*>(file)->uri().uri());
        ::fileSize(statResult);
        numReceivedMutex.lock();
        ++numReceived;
        if (numReceived == 3) {
            numReceivedMutex.unlock();
            quit();
        } else {
            numReceivedMutex.unlock();
        }
    }

    void fileData(const ByteStream &contents)
    {
        std::cout << contents;
    }

    void dirData(const List<Uri> &uriList)
    {
        IDEAL_SDEBUG("Found " << uriList.size() << " entries");
        IDEAL_SDEBUG("");
        List<Uri>::const_iterator it;
        for (it = uriList.begin(); it != uriList.end(); ++it) {
            Uri uri = *it;
            IDEAL_SDEBUG("Found entry: " << uri.uri());
        }
    }

private:
    iint32 numReceived;
    Mutex numReceivedMutex;
};

int main(int argc, char **argv)
{
    MyApplication app(argc, argv);

    IDEAL_SDEBUG("*** The first petition should take a bit longer than the rest because it has to");
    IDEAL_SDEBUG("    stablish the connection with the host. Since the host is the same for all");
    IDEAL_SDEBUG("    files, the protocol handler is shared for all them");
    IDEAL_SDEBUG("");

    {
        IDEAL_SDEBUG("*** File:\tftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.22.1.tar.gz");
        File f("ftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.22.1.tar.gz", &app);
        Object::connectStatic(f.statResult, fileSize);
        Thread *statJob = f.stat(Thread::Joinable);
        statJob->exec();
        statJob->join();
    }
    // from now on, petitions should be far faster, since we can reuse the existing connection
    {
        IDEAL_SDEBUG("*** File:\tftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.0.tar.gz");
        File f("ftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.0.tar.gz", &app);
        Object::connectStatic(f.statResult, fileSize);
        Thread *statJob = f.stat(Thread::Joinable);
        statJob->exec();
        statJob->join();
    }
    {
        IDEAL_SDEBUG("*** File:\tftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.8.tar.gz");
        File f("ftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.8.tar.gz", &app);
        Object::connectStatic(f.statResult, fileSize);
        Thread *statJob = f.stat(Thread::Joinable);
        statJob->exec();
        statJob->join();
    }
    {
        IDEAL_SDEBUG("*** File:\tftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.16.tar.gz");
        File f("ftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.16.tar.gz", &app);
        Object::connectStatic(f.statResult, fileSize);
        Thread *statJob = f.stat(Thread::Joinable);
        statJob->exec();
        statJob->join();
    }
    {
        IDEAL_SDEBUG("*** File:\tftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.29.1.tar.gz");
        File f("ftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.29.1.tar.gz", &app);
        Object::connectStatic(f.statResult, fileSize);
        Thread *statJob = f.stat(Thread::Joinable);
        statJob->exec();
        statJob->join();
    }
    {
        IDEAL_SDEBUG("*** File:\tfooishbar://foo");
        File f("fooishbar://foo", &app);
        Object::connectStatic(f.statResult, fileSize);
        Thread *statJob = f.stat(Thread::Joinable);
        statJob->exec();
        statJob->join();
    }
    {
        IDEAL_SDEBUG("*** File:\tftp://ftp.ereslibre.es");
        File f("ftp://ftp.ereslibre.es", &app);
        Object::connectStatic(f.statResult, fileSize);
        Object::connectStaticMulti(f.error, error);
        Thread *statJob = f.stat(Thread::Joinable);
        statJob->exec();
        statJob->join();
    }

    IDEAL_SDEBUG("");
    IDEAL_SDEBUG("*** Retrieving a Linux Kernel README synchronously");
    IDEAL_SDEBUG("");

    {
        File kernelReadme("http://www.kernel.org/pub/linux/kernel/README", &app);
        Object::connect(kernelReadme.dataRead, &app, &MyApplication::fileData);

        Thread *readmeContentsJob = kernelReadme.get(File::NoMaxBytes, Thread::Joinable);
        readmeContentsJob->exec();
        readmeContentsJob->join();

        IDEAL_SDEBUG("");
        IDEAL_SDEBUG("*** Retrieving only 1 KB of the README by http");
        IDEAL_SDEBUG("");

        Thread *contentsJob1KB = kernelReadme.get(1024, Thread::Joinable);
        contentsJob1KB->exec();
        contentsJob1KB->join();

        File kernelReadmeFtp("ftp://ftp.kernel.org/pub/linux/kernel/README", &app);
        Object::connect(kernelReadmeFtp.dataRead, &app, &MyApplication::fileData);

        IDEAL_SDEBUG("");
        IDEAL_SDEBUG("");
        IDEAL_SDEBUG("*** Retrieving the full README now by ftp");
        IDEAL_SDEBUG("");

        Thread *contentsJob = kernelReadmeFtp.get(File::NoMaxBytes, Thread::Joinable);
        contentsJob->exec();
        contentsJob->join();

        Uri bashrcUri(app.getPath(Application::Home), ".bashrc");
        File bashrc(bashrcUri, &app);
        Object::connect(bashrc.dataRead, &app, &MyApplication::fileData);

        IDEAL_SDEBUG("");
        IDEAL_SDEBUG("*** Retrieving " << bashrcUri.uri());
        IDEAL_SDEBUG("");

        contentsJob = bashrc.get(File::NoMaxBytes, Thread::Joinable);
        contentsJob->exec();
        contentsJob->join();
    }

    IDEAL_SDEBUG("");
    IDEAL_SDEBUG("*** Listing ftp://ftp.kernel.org/pub/");
    IDEAL_SDEBUG("");

    File listDir("ftp://ftp.kernel.org/pub/", &app);
    Object::connect(listDir.dataRead, &app, &MyApplication::fileData);
    Thread *listDirStat = listDir.get(File::NoMaxBytes, Thread::Joinable);
    listDirStat->exec();
    listDirStat->join();

    IDEAL_SDEBUG("");
    IDEAL_SDEBUG("*** Listing " << app.getPath(Application::Home));
    IDEAL_SDEBUG("");

    File listLocalDir(app.getPath(Application::Home), &app);
    Object::connect(listLocalDir.dirRead, &app, &MyApplication::dirData);
    Thread *listLocalDirStat = listLocalDir.get(File::NoMaxBytes, Thread::Joinable);
    listLocalDirStat->exec();
    listLocalDirStat->join();

    IDEAL_SDEBUG("");
    IDEAL_SDEBUG("*** Completely asynchronous way coming");
    IDEAL_SDEBUG("");

    File f1("ftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.22.1.tar.gz", &app);
    Object::connectMulti(f1.statResult, &app, &MyApplication::fileSize);
    File f2("ftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.0.tar.gz", &app);
    Object::connectMulti(f2.statResult, &app, &MyApplication::fileSize);
    File f3("ftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.29.1.tar.gz", &app);
    Object::connectMulti(f3.statResult, &app, &MyApplication::fileSize);
    Thread *statJob = f1.stat();
    statJob->exec();
    statJob = f2.stat();
    statJob->exec();
    statJob = f3.stat();
    statJob->exec();

    return app.exec();
}
