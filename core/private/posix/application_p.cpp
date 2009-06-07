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

#include <stdlib.h>
#include <signal.h>
#include <algorithm>
#include <locale.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <map>

#include <core/application.h>
#include "application_p.h"

#include <core/timer.h>
#include "timer_p.h"

#include <core/private/option_p.h>
#include <core/private/module_p.h>

namespace IdealCore {

static void signal_recv(int signum, siginfo_t *info, void *ptr)
{
    switch (signum) {
        case SIGILL:
            IDEAL_SDEBUG("*** This program has failed (SIGILL). It cannot continue.");
            break;
        case SIGABRT:
            IDEAL_SDEBUG("*** This program has failed (SIGABRT). It cannot continue.");
            break;
        case SIGFPE:
            IDEAL_SDEBUG("*** This program has failed (SIGFPE). It cannot continue.");
            break;
        case SIGSEGV:
            IDEAL_SDEBUG("*** This program has failed (SIGSEGV). It cannot continue.");
            break;
        case SIGPIPE:
            IDEAL_SDEBUG("*** This program has failed (SIGPIPE). It cannot continue.");
            break;
        default:
            IDEAL_SDEBUG("*** This program has failed (UNKNOWN). It cannot continue.");
            break;
    }
    IDEAL_SDEBUG("*** Please, contact developers with the following information:");
    int fd;
    char filename[L_tmpnam];
    do {
        if (tmpnam(filename)) {} // tmpnam has attribute "warn_unused_result" for some bad reason...
    } while ((fd = open(filename, O_WRONLY | O_CREAT | O_EXCL, 0600)) < 0);
    const pid_t pid = fork();
    switch (pid) {
        case 0:
            dprintf(fd, "attach %d\nbt\n", getppid());
            close(fd);
            execlp("gdb", "gdb", "-batch", "-x", filename, (char*) 0);
        default:
            close(fd);
            waitpid(pid, NULL, 0);
            unlink(filename);
            exit(EXIT_FAILURE);
    }
}

Application::PrivateImpl::PrivateImpl(Application *q)
    : Private(q)
{
    setlocale(LC_ALL, "");
#if 0
    {
        struct sigaction sa;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sa.sa_handler = SIG_IGN;
        sigaction(SIGINT, &sa, NULL);
    }
#endif
    {
        struct sigaction sa;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sa.sa_sigaction = signal_recv;
        sa.sa_flags = SA_SIGINFO;
        sigaction(SIGILL, &sa, NULL);
    }
    {
        struct sigaction sa;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sa.sa_sigaction = signal_recv;
        sa.sa_flags = SA_SIGINFO;
        sigaction(SIGABRT, &sa, NULL);
    }
    {
        struct sigaction sa;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sa.sa_sigaction = signal_recv;
        sa.sa_flags = SA_SIGINFO;
        sigaction(SIGFPE, &sa, NULL);
    }
    {
        struct sigaction sa;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sa.sa_sigaction = signal_recv;
        sa.sa_flags = SA_SIGINFO;
        sigaction(SIGSEGV, &sa, NULL);
    }
    {
        struct sigaction sa;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sa.sa_sigaction = signal_recv;
        sa.sa_flags = SA_SIGINFO;
        sigaction(SIGPIPE, &sa, NULL);
    }
}

void Application::Private::addOptionWithoutArg(Option &option, char optChar, const char *longOpt)
{
    PrivateImpl::OptionItem optionItem;
    {
        struct option opt;
        opt.name = longOpt;
        opt.has_arg = no_argument;
        opt.flag = 0;
        opt.val = optChar;
        optionItem.opt = opt;
    }
    optionItem.option = &option;
    D_I->m_optionList.push_back(optionItem);
}

void Application::Private::addOptionWithMandatoryArg(Option &option, char optChar, const char *longOpt)
{
    PrivateImpl::OptionItem optionItem;
    {
        struct option opt;
        opt.name = longOpt;
        opt.has_arg = required_argument;
        opt.flag = 0;
        opt.val = optChar;
        optionItem.opt = opt;
    }
    optionItem.option = &option;
    D_I->m_optionList.push_back(optionItem);
}

void Application::Private::addOptionWithOptionalArg(Option &option, char optChar, const char *longOpt)
{
    PrivateImpl::OptionItem optionItem;
    {
        struct option opt;
        opt.name = longOpt;
        opt.has_arg = optional_argument;
        opt.flag = 0;
        opt.val = optChar;
        optionItem.opt = opt;
    }
    optionItem.option = &option;
    D_I->m_optionList.push_back(optionItem);
}

List<char*> Application::Private::parseOptions(ParsingStrictness parsingStrictness, FailVerbosity failVerbosity)
{
    if (failVerbosity == FailSilently) {
        opterr = 0;
    }
    std::string shortopts;
    struct option longopts[D_I->m_optionList.size() + 1];
    std::map<char, PrivateImpl::OptionItem> optionMap;
    List<PrivateImpl::OptionItem>::iterator it;
    int i = 0;
    for (it = D_I->m_optionList.begin(); it != D_I->m_optionList.end(); ++it, ++i) {
        PrivateImpl::OptionItem optionItem = *it;
        if (optionItem.opt.has_arg == no_argument) {
            shortopts += optionItem.opt.val;
        } else if (optionItem.opt.has_arg == required_argument) {
            shortopts += optionItem.opt.val + ':';
        } else {
            shortopts += optionItem.opt.val + "::";
        }
        longopts[i] = optionItem.opt;
        optionMap[optionItem.opt.val] = optionItem;
    }
    struct option option;
    option.name = 0;
    option.has_arg = 0;
    option.flag = 0;
    option.val = 0;
    longopts[D_I->m_optionList.size()] = option;
    int opt;
    if (parsingStrictness == Flexible) {
        while ((opt = getopt_long_only(m_argc, m_argv, shortopts.data(), longopts, NULL)) != -1) {
            if (opt == '?') {
                q->emit(q->invalidOption);
            } else if (opt == ':') {
                q->emit(q->missingParameter);
            } else if (optionMap.count(opt)) {
                PrivateImpl::OptionItem item = optionMap[opt];
                item.option->d->m_isOptSet = true;
                item.option->d->m_optArg = optarg;
            }
        }
    } else {
        while ((opt = getopt_long(m_argc, m_argv, shortopts.data(), longopts, NULL)) != -1) {
            if (opt == '?') {
                q->emit(q->invalidOption);
            } else if (opt == ':') {
                q->emit(q->missingParameter);
            } else if (optionMap.count(opt)) {
                PrivateImpl::OptionItem item = optionMap[opt];
                item.option->d->m_isOptSet = true;
                item.option->d->m_optArg = optarg;
            }
        }
    }
    for (it = D_I->m_optionList.begin(); it != D_I->m_optionList.end(); ++it) {
        PrivateImpl::OptionItem item = *it;
        item.option->d->m_isValid = true;
    }
    List<char*> res;
    for (; optind < m_argc; ++optind) {
        res.push_back(m_argv[optind]);
    }
    return res;
}

String Application::Private::getPath(Path path) const
{
    switch (path) {
        case Prefix:
            return IDEALLIBRARY_PREFIX;
        case Global:
            return getenv("PATH");
        case Library:
            return getenv("LD_LIBRARY_PATH");
        case PkgConfig:
            return getenv("PKG_CONFIG_PATH");
        case Home:
            return getenv("HOME");
        case Module:
            return IDEALLIBRARY_PREFIX "/lib/ideal";
#ifndef NDEBUG
        case UnitTest:
            return String(getenv("HOME")) + "/.ideal-unittests";
#endif
        default:
            return String();
    }
}

void Application::Private::processEvents()
{
    int timeToSleep;
    if (m_sleepTime != -1) {
        checkTimers();
        timeToSleep = m_sleepTime;
    } else {
        m_runningTimerListMutex.lock();
        if (!m_runningTimerList.empty()) {
            m_sleepTime = m_runningTimerList.front()->d->m_interval;
            timeToSleep = m_sleepTime;
        } else {
            timeToSleep = m_defaultSleepTime;
        }
        m_runningTimerListMutex.unlock();
    }
    struct timespec tts;
    tts.tv_sec = timeToSleep / 1000;
    tts.tv_nsec = (timeToSleep % 1000) * 1000000;
    nanosleep(&tts, 0);
}

void Application::Private::processDelayedDeletions()
{
    List<Object*>::iterator it;
    m_markedForDeletionMutex.lock();
    for (it = m_markedForDeletion.begin(); it != m_markedForDeletion.end(); ++it) {
        delete *it;
    }
    m_markedForDeletion.clear();
    m_markedForDeletionMutex.unlock();
}

void Application::Private::unloadUnneededDynamicLibraries()
{
    List<IdealCore::Module*>::iterator it;
    m_markedForUnloadMutex.lock();
    for (it = m_markedForUnload.begin(); it != m_markedForUnload.end(); ++it) {
        IdealCore::Module *module = *it;
        if (module->d->m_unused || !module->d->m_refs) {
            void *handle = module->d->m_handle;
            module->d->m_unused = false;
            delete module;
            dlclose(handle);
        }
    }
    m_markedForUnload.clear();
    m_markedForUnloadMutex.unlock();
}

bool Application::PrivateImpl::timerSort(const Timer *left, const Timer *right)
{
    return left->d->m_remaining < right->d->m_remaining;
}

void Application::Private::quit()
{
    exit(EXIT_SUCCESS);
}

void Application::Private::checkTimers()
{
    m_runningTimerListMutex.lock();
    if (m_runningTimerList.empty()) {
        return;
    }
    List<Timer*> expiredTimerList;
    {
        Timer *const firstExpiredTimer = m_runningTimerList.front();
        {
            // We control here the case in which the timer that was going to expire
            // (the head of the queue) was removed before actually expiring. With this
            // check we won't fire an incorrect timer when it had already time remaining.
            const int msDelta = firstExpiredTimer->d->m_remaining - m_sleepTime;
            if (msDelta) {
                std::vector<Timer*>::iterator it;
                for (it = m_runningTimerList.begin(); it != m_runningTimerList.end(); ++it) {
                    Timer *const currTimer = *it;
                    currTimer->d->m_remaining -= m_sleepTime;
                }
                m_sleepTime = std::min(msDelta, m_defaultSleepTime);
                m_runningTimerListMutex.unlock();
                return;
            }
        }
        m_nextTimeout = firstExpiredTimer->d->m_remaining;
        std::vector<Timer*>::iterator it = m_runningTimerList.begin();
        while (it != m_runningTimerList.end()) {
            Timer *const currTimer = *it;
            if (currTimer->d->m_remaining == firstExpiredTimer->d->m_remaining) {
                if (currTimer->d->m_timeoutType == Timer::SingleShot) {
                    currTimer->d->m_state = Timer::Stopped;
                    it = m_runningTimerList.erase(it);
                } else {
                    currTimer->d->m_remaining = currTimer->d->m_interval;
                    ++it;
                }
                expiredTimerList.push_back(currTimer);
            } else {
                break;
            }
        }
        if (!m_runningTimerList.empty()) {
            while (it != m_runningTimerList.end()) {
                Timer *const currTimer = *it;
                currTimer->d->m_remaining -= m_nextTimeout;
                ++it;
            }
        }
    }
    if (!m_runningTimerList.empty()) {
        std::sort(m_runningTimerList.begin(), m_runningTimerList.end(), PrivateImpl::timerSort);
        Timer *const nextTimer = m_runningTimerList.front();
        m_sleepTime = nextTimer->d->m_remaining;
    } else {
        m_sleepTime = -1;
    }
    m_runningTimerListMutex.unlock();
    List<Timer*>::iterator it;
    for (it = expiredTimerList.begin(); it != expiredTimerList.end(); ++it) {
        Timer *const currTimer = *it;
        currTimer->emit(currTimer->timeout);
    }
}

}
