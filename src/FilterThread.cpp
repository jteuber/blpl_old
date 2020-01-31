
#include "FilterThread.h"

/// FILTER

/**
 * @brief Constructor.
 *
 * @param pOutput       Pipe on which the filter will dump its outgoing data.
 * @param pFilter       The filter that is called for processing in this thread.
 * @param pInput        Pipe which stores the input for the filter.
 * @param bSelfManaged  True if the FilterThread is allowed to manage the filter by itself, false if another class manages the FilterThread (i.e. calls FilterThread::run()).
 */
FilterThread::FilterThread(Pipe* pOutput,
                           Filter* pFilter,
                           Pipe* pInput,
                           bool bSelfManaged)
    : m_pOutput(pOutput)
    , m_bFilterThreadActive(true)
    , m_bSelfManaged(bSelfManaged)
    , m_pFilter(pFilter)
    , m_pInput(pInput)
{
}

/**
 * @brief Destructor, stops the thread.
 */
FilterThread::~FilterThread()
{
    if (m_bFilterThreadActive)
        FilterThread::stop();
}


/**
 * @brief Method that is called by the thread, waits for input data and calls the filters process method.
 */
void FilterThread::run()
{
    m_bFilterThreadActive = true;
    do {
        std::shared_ptr<PipeData> pTemp = m_pInput->blockingPop();
        if (pTemp)
            pTemp = m_pFilter->process(pTemp);

        if (pTemp)
            m_pOutput->push(pTemp);
        else
            m_pInput->reset();
    } while (m_bFilterThreadActive && m_bSelfManaged);
}

/**
 * @brief Returns whether the filter is currently working on data.
 *
 * @return True, if the thread is active and the filter is working, false if not.
 */
bool FilterThread::isFiltering()
{
    return m_bFilterThreadActive;
}

/**
 * @brief Stops the thread and joins it with the calling one.
 */
void FilterThread::stop()
{
    m_bFilterThreadActive = false;
    m_pInput->disable();
    m_pInput->reset();

    join();

    m_pInput->enable();
}

/// MULTIFILTER

/**
 * @brief Constructor.
 *
 * @param pOutput       Pipe on which the filter will dump its outgoing data.
 * @param pFilter       The filter that is called for processing in this thread.
 * @param inputPipes    Arbitrary number of input pipes storing the input for the filter.
 */
MultiFilterThread::MultiFilterThread(Pipe* pOutput,
                                     MultiFilter* pMultiFilter,
                                     std::vector<Pipe*>::iterator begin,
                                     std::vector<Pipe*>::iterator end,
                                     bool bSelfManaged)
    : FilterThread(pOutput, nullptr, nullptr, bSelfManaged)
    , m_pFilter(pMultiFilter)
    , m_inputPipes(begin, end)
{
}

/**
 * @brief Destructor, also stops the thread.
 */
MultiFilterThread::~MultiFilterThread()
{
    if (m_bFilterThreadActive)
        MultiFilterThread::stop();
}

/**
 * @brief Method that is called by the thread, waits for input data and calls the filters process method.
 */
void MultiFilterThread::run()
{
    m_bFilterThreadActive = true;
    do {
        // prepare vector for the data of all incoming pipes
        std::vector<std::shared_ptr<PipeData>> incoming;
        incoming.resize(m_inputPipes.size());

        // get the data from the pipes
        for (unsigned int i = 0; i < m_inputPipes.size(); ++i)
            incoming[i] = m_inputPipes[i]->blockingPop();

        if (!m_bFilterThreadActive)
            return;

        // process the data
        std::shared_ptr<PipeData> pTemp = m_pFilter->processImpl(incoming);
        if (pTemp)
            m_pOutput->push(pTemp);
    } while (m_bFilterThreadActive && m_bSelfManaged);
}

/**
 * @brief Stops the thread and joins it with the calling one.
 */
void MultiFilterThread::stop()
{
    m_bFilterThreadActive = false;
    for (auto inputPipe : m_inputPipes)
        inputPipe->disable();
    for (auto inputPipe : m_inputPipes)
        inputPipe->reset();

    join();
    for (auto inputPipe : m_inputPipes)
        inputPipe->enable();
}
