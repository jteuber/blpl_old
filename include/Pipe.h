#pragma once

#include "Pipeline_global.h"

#include <atomic>
#include <thread>

/**
 * Implements the pipes in the pipeline.
 * @tparam TData Type of the data to pass through the Pipe.
 */
template <typename TData>
class PIPELINE_EXPORT Pipe {
public:
    explicit Pipe(bool waitForSlowestFilter = false);
    virtual ~Pipe() = default;

    TData pop();
    TData blockingPop();

    void push(TData &&data);
    void reset();

    void disable();
    void enable();

    unsigned int size();

private:
    TData m_elem;
    std::atomic<bool> m_valid;

    bool m_waitForSlowestFilter;
    bool m_enabled;
};

template <typename TData>
Pipe<TData>::Pipe(bool waitForSlowestFilter)
    : m_valid(false)
    , m_waitForSlowestFilter(waitForSlowestFilter)
    , m_enabled(true)
{}

template <typename TData>
TData Pipe<TData>::pop()
{
    TData temp = std::move(m_elem);
    m_valid    = false;
    return temp;
}

template <typename TData>
TData Pipe<TData>::blockingPop()
{
    while (!m_valid && m_enabled)
        std::this_thread::yield();

    return pop();
}

template <typename TData>
void Pipe<TData>::push(TData&& data)
{
    while (m_waitForSlowestFilter && m_valid && m_enabled)
        std::this_thread::yield();

    if (!m_enabled)
        return;

    m_elem  = std::move(data);
    m_valid = true;
}

template <typename TData>
void Pipe<TData>::reset()
{
    m_valid = false;
}

template <typename TData>
void Pipe<TData>::disable()
{
    m_enabled = false;
}

template <typename TData>
void Pipe<TData>::enable()
{
    m_enabled = true;
}

template <typename TData>
unsigned int Pipe<TData>::size()
{
    return m_valid ? 1 : 0;
}
