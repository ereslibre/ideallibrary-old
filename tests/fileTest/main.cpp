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

#include <core/application.h>
#include <core/thread.h>
#include <core/file.h>
#include <core/uri.h>

using namespace IdealCore;

static void fileSize(double size)
{
    IDEAL_SDEBUG("\tSize:\t" << (size / (1024 * 1024)) << " MiB");
}

static void error(Object *file, ProtocolHandler::ErrorCode errorCode)
{
    IDEAL_SDEBUG("\tError (" <<  errorCode << ") reading: " << static_cast<File*>(file)->uri().uri());
}

class MyApplication
    : public Application
{
public:
    MyApplication(int argc, char **argv)
        : Application(argc, argv)
        , numReceived(0)
    {
    }

    void fileSize(Object *file, double size)
    {
        IDEAL_SDEBUG("*** File:\t" << static_cast<File*>(file)->uri().uri());
        ::fileSize(size);
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

private:
    int numReceived;
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
        Object::connectStatic(f.sizeResult, fileSize);
        Thread *sizeJob = f.size(Thread::Joinable);
        sizeJob->exec();
        sizeJob->join();
    }
    // from now on, petitions should be far faster, since we can reuse the existing connection
    {
        IDEAL_SDEBUG("*** File:\tftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.0.tar.gz");
        File f("ftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.0.tar.gz", &app);
        Object::connectStatic(f.sizeResult, fileSize);
        Thread *sizeJob = f.size(Thread::Joinable);
        sizeJob->exec();
        sizeJob->join();
    }
    {
        IDEAL_SDEBUG("*** File:\tftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.8.tar.gz");
        File f("ftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.8.tar.gz", &app);
        Object::connectStatic(f.sizeResult, fileSize);
        Thread *sizeJob = f.size(Thread::Joinable);
        sizeJob->exec();
        sizeJob->join();
    }
    {
        IDEAL_SDEBUG("*** File:\tftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.16.tar.gz");
        File f("ftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.16.tar.gz", &app);
        Object::connectStatic(f.sizeResult, fileSize);
        Thread *sizeJob = f.size(Thread::Joinable);
        sizeJob->exec();
        sizeJob->join();
    }
    {
        IDEAL_SDEBUG("*** File:\tftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.29.1.tar.gz");
        File f("ftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.29.1.tar.gz", &app);
        Object::connectStatic(f.sizeResult, fileSize);
        Thread *sizeJob = f.size(Thread::Joinable);
        sizeJob->exec();
        sizeJob->join();
    }
    {
        IDEAL_SDEBUG("*** File:\tfooishbar://foo");
        File f("fooishbar://foo", &app);
        Object::connectStatic(f.sizeResult, fileSize);
        Thread *sizeJob = f.size(Thread::Joinable);
        sizeJob->exec();
        sizeJob->join();
    }
    {
        IDEAL_SDEBUG("*** File:\tftp://ftp.ereslibre.es");
        File f("ftp://ftp.ereslibre.es", &app);
        Object::connectStatic(f.sizeResult, fileSize);
        Object::connectStaticMulti(f.error, error);
        Thread *sizeJob = f.size(Thread::Joinable);
        sizeJob->exec();
        sizeJob->join();
    }

    IDEAL_SDEBUG("");
    IDEAL_SDEBUG("*** Retrieving a Linux Kernel README synchronously");
    IDEAL_SDEBUG("");

    {
        File kernelReadme("http://www.kernel.org/pub/linux/kernel/README", &app);
        Object::connect(kernelReadme.dataRead, &app, &MyApplication::fileData);

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

        Thread *contentsJob = kernelReadmeFtp.get(ProtocolHandler::NoMaxBytes, Thread::Joinable);
        contentsJob->exec();
        contentsJob->join();

        File stdio("file:///usr/include/stdio.h", &app);
        Object::connect(stdio.dataRead, &app, &MyApplication::fileData);

        IDEAL_SDEBUG("");
        IDEAL_SDEBUG("*** Retrieving /usr/include/stdio.h");
        IDEAL_SDEBUG("");

        contentsJob = stdio.get(ProtocolHandler::NoMaxBytes, Thread::Joinable);
        contentsJob->exec();
        contentsJob->join();

    }

    IDEAL_SDEBUG("");
    IDEAL_SDEBUG("*** Completely asynchronous way coming");
    IDEAL_SDEBUG("");

    File f1("ftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.22.1.tar.gz", &app);
    Object::connectMulti(f1.sizeResult, &app, &MyApplication::fileSize);
    File f2("ftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.0.tar.gz", &app);
    Object::connectMulti(f2.sizeResult, &app, &MyApplication::fileSize);
    File f3("ftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.29.1.tar.gz", &app);
    Object::connectMulti(f3.sizeResult, &app, &MyApplication::fileSize);
    Thread *sizeJob = f1.size();
    sizeJob->exec();
    sizeJob = f2.size();
    sizeJob->exec();
    sizeJob = f3.size();
    sizeJob->exec();

    return app.exec();
}
