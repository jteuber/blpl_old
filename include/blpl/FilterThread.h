#pragma once

#include "Pipeline_global.h"

#include <memory>
#include <mutex>

#include "AbstractPipe.h"
#include "Filter.h"
#include "Pipe.h"

namespace blpl {

class PIPELINE_EXPORT AbstractFilterThread
{
public:
    virtual void start() = 0;
    virtual void stop()  = 0;
};

/**
 * @brief The FilterThread class encapsulates a filter and manages it to work on
 * all incoming data in a separate thread.
 *
 * One instance of this class only uses one thread at a time, so each filter
 * works on its incoming data sequentially. By using this on all filters of the
 * pipeline, the pipeline can work on all stages in parallel while the
 * individual stages don't have to be threadsafe.
 */
template <class InData, class OutData>
class PIPELINE_EXPORT FilterThread : public AbstractFilterThread
{
public:
    explicit FilterThread(std::shared_ptr<Pipe<InData>> inPipe,
                          std::shared_ptr<Filter<InData, OutData>> filter,
                          std::shared_ptr<Pipe<OutData>> outPipe,
                          bool bSelfManaged = true);
    ~FilterThread();

    virtual bool isFiltering();

    void start() override;
    void stop() override;

private:
    void run();

private:
    std::shared_ptr<Pipe<InData>> m_inPipe;
    std::shared_ptr<Filter<InData, OutData>> m_filter;
    std::shared_ptr<Pipe<OutData>> m_outPipe;

    volatile bool m_bFilterThreadActive;
    volatile bool m_bFiltering;
    bool m_bSelfManaged;
    std::thread m_thread;
};

/**
 * @brief Constructor.
 *
 * @param pOutput       Pipe on which the filter will dump its outgoing data.
 * @param pFilter       The filter that is called for processing in this thread.
 * @param pInput        Pipe which stores the input for the filter.
 * @param bSelfManaged  True if the FilterThread is allowed to manage the filter
 * by itself, false if another class manages the FilterThread (i.e. calls
 * FilterThread::run()).
 */
template <class InData, class OutData>
FilterThread<InData, OutData>::FilterThread(
    std::shared_ptr<Pipe<InData>> inPipe,
    std::shared_ptr<Filter<InData, OutData>> filter,
    std::shared_ptr<Pipe<OutData>> outPipe,
    bool bSelfManaged)
    : m_inPipe(inPipe)
    , m_filter(filter)
    , m_outPipe(outPipe)
    , m_bFilterThreadActive(false)
    , m_bFiltering(false)
    , m_bSelfManaged(bSelfManaged)
{}

/**
 * @brief Destructor, stops the thread.
 */
template <class InData, class OutData>
FilterThread<InData, OutData>::~FilterThread()
{
    if (m_bFiltering)
        FilterThread<InData, OutData>::stop();
}

/**
 * @brief Returns whether the filter is currently working on data.
 *
 * @return True, if the thread is active and the filter is working, false if
 * not.
 */
template <class InData, class OutData>
bool FilterThread<InData, OutData>::isFiltering()
{
    return m_bFiltering;
}

/**
 * @brief Starts the filter.
 */
template <class InData, class OutData>
void FilterThread<InData, OutData>::start()
{
    m_thread = std::thread(&FilterThread<InData, OutData>::run, this);
}

/**
 * @brief Stops the thread and joins it with the calling one.
 */
template <class InData, class OutData>
void FilterThread<InData, OutData>::stop()
{
    if (m_bFiltering) {
        m_bFilterThreadActive = false;
        m_inPipe->reset();
        m_inPipe->disable();

        m_thread.join();

        m_inPipe->enable();
    }
}

/**
 * @brief Method that is called by the thread, waits for input data and calls
 * the filters process method.
 */
template <class InData, class OutData>
void FilterThread<InData, OutData>::run()
{
    m_bFilterThreadActive = true;
    m_bFiltering          = true;
    do {
        InData temp = m_inPipe->blockingPop();
        if (m_bFilterThreadActive)
            m_outPipe->push(m_filter->process(std::move(temp)));
    } while (m_bFilterThreadActive && m_bSelfManaged);
    m_bFiltering = false;
}

}
