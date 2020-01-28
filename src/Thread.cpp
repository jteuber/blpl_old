
/*
Object Oriented Communication Library
Copyright (c) 2011 Jürgen Lorenz and Jörn Teuber

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use
of this software. Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim
that you wrote the original software. If you use this software in a product, an
acknowledgment in the product documentation would be appreciated but is not
required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/
// This file was written by Jürgen Lorenz and Jörn Teuber

#include "Thread.h"

int Thread::sm_iThreadCount = 0;

/**
 * @fn	Thread::Thread()
 *
 * @brief	Default constructor.
 */
Thread::Thread()
    : m_bActive(false)
    , m_iThreadPriority(TP_Normal)
#ifndef WIN32
    , m_iThreadID(0)
#else
    , m_iThreadID(0)
    , m_hThread(0)
#endif
{}

/**
 * @fn	Thread::Thread()
 *
 * @brief	Default constructor.
 */
Thread::~Thread()
{
    join();
    m_bActive = false;
}

/**
 * @fn	void Thread::join()
 *
 * @brief	blocks until the thread has finished.
 */
void Thread::join()
{
    if (m_bActive) {
#ifndef WIN32
        pthread_join(m_iThreadID, nullptr);
#else
        WaitForSingleObject(m_hThread, INFINITE);
#endif
    }
}

/**
 * @fn	void Thread::sleep( int iMilliseconds )
 *
 * @brief	Blocks the thread for a specified time.
 *
 * @note	Only call this from _inside_ the thread method!
 *
 * @param	iMilliseconds	milliseconds to sleep or 0 to pass the current CPU
 * timeslot to the next thread or process.
 */
void Thread::sleep(unsigned int iMilliseconds)
{
#ifndef WIN32
    usleep(iMilliseconds * 1000);
#else
    Sleep(static_cast<DWORD>(iMilliseconds));
#endif
}

/**
 * @fn	bool Thread::isAlive()
 *
 * @brief	Query whether this thread is alive.
 *
 * @return	true if alive, false if not.
 */
bool Thread::isAlive() const { return m_bActive; }

/**
 * @fn	bool Thread::start()
 *
 * @brief	Starts the thread.
 *
 * @return	true if it succeeds, false if it fails.
 */
bool Thread::start()
{
    join();
#ifndef WIN32
    return !pthread_create(&m_iThreadID, nullptr, Thread::entryPoint, this);
#else
    m_hThread =
        CreateThread(nullptr, 0, Thread::entryPoint, this, 0, &m_iThreadID);
    return (m_hThread);
#endif
}

#ifndef WIN32
void* Thread::entryPoint(void* pthis)
#else
DWORD WINAPI Thread::entryPoint(LPVOID pthis)
#endif
{
    Thread* pThread    = static_cast<Thread*>(pthis);
    pThread->m_bActive = true;
    pThread->run();
    pThread->m_bActive = false;

    return nullptr;
}

/**
 * @fn	void Thread::setPriority( EPriority iPriority )
 *
 * @brief	Sets a new thread priority.
 *
 * @param	iPriority	the new priority.
 */
void Thread::setPriority(EPriority iPriority)
{
    m_iThreadPriority = iPriority;

#ifndef WIN32
    struct sched_param sp;
    memset(&sp, 0, sizeof(struct sched_param));

    int iMin = sched_get_priority_min(0);
    int iMax = sched_get_priority_max(0);
    int iAvg = (iMax + iMin) / 2;

    sp.sched_priority = iAvg + (iPriority * (iMax - iMin)) / 4;

    pthread_setschedparam(m_iThreadID, SCHED_RR, &sp);
#else
    SetThreadPriority(&m_hThread, iPriority);
#endif
}

/**
 * @brief Returns the current priority of this thread.
 */
Thread::EPriority Thread::getPriority() const { return m_iThreadPriority; }
