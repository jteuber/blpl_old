
#include "Pipe.h"

#include <memory>

#include "Log.h"

Pipe::Pipe(bool waitForSlowestFilter, bool bBuffered)
    : m_bBuffered(bBuffered)
    , m_waitForSlowestFilter(waitForSlowestFilter)
    , m_bEnabled(true)
{
    m_spElem = nullptr;
}

std::shared_ptr<PipeData> Pipe::pop()
{
    m_mutex.lock();
    if (m_spElem) {
        std::shared_ptr<PipeData> pRet = m_spElem;

        if (!m_bBuffered)
            m_spElem.reset();

        m_mutex.unlock();
        return pRet;
    }
    m_mutex.unlock();

    return nullptr;
}

std::shared_ptr<PipeData> Pipe::blockingPop()
{
    while (size() == 0 && m_bEnabled)
        Thread::sleep();

    return pop();
}

void Pipe::push( const std::shared_ptr<PipeData>& pData )
{
    while (!m_bBuffered && m_waitForSlowestFilter && m_spElem && m_bEnabled)
        Thread::sleep();

    if (!m_bEnabled)
        return;

    m_mutex.lock();
    if (m_spElem)
        m_spElem.reset();
    m_spElem = pData;
    m_mutex.unlock();
}

void Pipe::reset()
{
    m_mutex.lock();
    m_spElem.reset();
    m_mutex.unlock();
}

void Pipe::disable()
{
    m_bEnabled = false;
}

void Pipe::enable()
{
    m_bEnabled = true;
}

unsigned int Pipe::size()
{
    // m_mutex.lock();
    unsigned int uiRet = m_spElem ? 1 : 0;
    // m_mutex.unlock();

    return uiRet;
}

NullPipe::NullPipe(int msecsBetweenPops)
    : m_wait(msecsBetweenPops > 0)
    , m_msecsBetweenPops(msecsBetweenPops)
    , m_lastPop(std::chrono::high_resolution_clock::now())
{

}

std::shared_ptr<PipeData> NullPipe::pop()
{
    return std::make_shared<PipeData>();
}

std::shared_ptr<PipeData> NullPipe::blockingPop()
{
    if (m_wait) {
        std::chrono::high_resolution_clock::time_point tNow =
            std::chrono::high_resolution_clock::now();
        std::chrono::duration<int, std::milli> time_span =
            std::chrono::duration_cast<std::chrono::duration<int, std::milli>>(
                tNow - m_lastPop);

        Thread::sleep(static_cast<unsigned int>(
            std::max(m_msecsBetweenPops - time_span.count(), 0)));
        m_lastPop = std::chrono::high_resolution_clock::now();
    }

    return std::make_shared<PipeData>();
}
