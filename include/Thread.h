#ifndef THREAD_H_INCLUDED
#define THREAD_H_INCLUDED

#include "Pipeline_global.h"

#ifndef WIN32
#include <cstring>
#include <pthread.h>
#include <unistd.h>
#else
#define NOMINMAX
#include <windows.h>
#endif

/**
 * @class	Thread
 *
 * @brief	A cross-plattform implementation of a simple thread.
 */
class PIPELINE_EXPORT Thread
{
public:
    /**
     * @enum	EPriority
     *
     * @brief	Values that represent the thread priority.
     */
    enum EPriority
    {
        TP_Lowest = -2,
        TP_Low,
        TP_Normal,
        TP_High,
        TP_Highest
    };

    Thread();
    virtual ~Thread();

    bool start();
    void join();

    // setter
    void setPriority(EPriority iPriority);

    // getter
    [[nodiscard]] EPriority getPriority() const;
    [[nodiscard]] bool isAlive() const;

    // for calling from inside the thread
    static void sleep(unsigned int iMilliseconds = 0);

protected:
    /**
     * @brief This method is called in the thread after it is started with
     * start().
     */
    virtual void run() = 0;

#ifndef WIN32
    static void* entryPoint(void* pthis);
#else
    static DWORD WINAPI entryPoint(LPVOID runnableInstance);
#endif

private:
    volatile bool m_bActive;

    ///< The thread priority
    EPriority m_iThreadPriority;

#ifndef WIN32
    ///< Identifier for the thread
    pthread_t m_iThreadID;
#else
    ///< Identifier for the thread
    DWORD m_iThreadID;
    ///< Handle of the thread
    HANDLE m_hThread;
#endif

    ///< Number of threads
    static int sm_iThreadCount;
};

#endif // THREAD_H_INCLUDED
