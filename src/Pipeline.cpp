#include "Pipeline.h"

#include "Log.h"
#include "utils.h"

Pipeline::Pipeline(bool waitForSlowestFilter, bool useThreading,
                   int msecsBetweenFrames)
    : m_lastPipeID(0), m_waitForSlowestFilter(waitForSlowestFilter),
      m_bUseThreading(useThreading), m_msecsBetweenFrames(msecsBetweenFrames),
      m_nrOfSources(0) {}

Pipeline::~Pipeline() {
  for (auto thread : m_filterThreads)
    SAFE_DELETE(thread);

  for (auto pipe : m_pipes)
    SAFE_DELETE(pipe)
}

void Pipeline::addParallelFilters(std::vector<Filter *> &filters) {
  // create the starting pipes if none exist
  if (m_pipes.empty()) {
    for (unsigned int i = 0; i < filters.size(); ++i)
      m_pipes.push_back(new NullPipe(m_msecsBetweenFrames));
    m_nrOfSources = filters.size();
  } else if (m_nrOfSources != filters.size())
    Log::getDefaultLog() << Log::EL_ERROR
                         << "tried to add different number of parallel filters "
                            "than the first add!"
                         << Log::endl;

  unsigned int outputPipeStart = m_pipes.size();
  unsigned int inputPipeStart = m_pipes.size() - filters.size();

  // create the output pipes
  for (unsigned int i = 0; i < m_nrOfSources; ++i)
    m_pipes.push_back(new Pipe(m_waitForSlowestFilter));

  // insert the filters
  for (unsigned int i = 0; i < m_nrOfSources; ++i) {
    m_filterThreads.push_back(
        new FilterThread(m_pipes[i + outputPipeStart], filters[i],
                         m_pipes[i + inputPipeStart], m_bUseThreading));
    m_filters.push_back(filters[i]);
  }

  m_lastPipeID = m_pipes.size() - 1;
}

void Pipeline::addMultiFilter(MultiFilter *pFilter) {
  m_pipes.push_back(new Pipe(m_waitForSlowestFilter));
  m_filterThreads.push_back(new MultiFilterThread(
      m_pipes[m_lastPipeID + 1], pFilter, m_pipes.end() - m_nrOfSources - 1,
      m_pipes.end() - 1, m_bUseThreading));
  ++m_lastPipeID;

  m_multiFilters.push_back(pFilter);
}

void Pipeline::addFilter(Filter *pFilter) {
  m_pipes.push_back(new Pipe(m_waitForSlowestFilter));
  m_filterThreads.push_back(new FilterThread(m_pipes[m_lastPipeID + 1], pFilter,
                                             m_pipes[m_lastPipeID],
                                             m_bUseThreading));
  ++m_lastPipeID;

  m_filters.push_back(pFilter);
}

bool Pipeline::start() {
  bool bRet = true;

  if (m_bUseThreading) {
    // check for threads that are still alive, as that shouldn't happen
    for (auto thread : m_filterThreads)
      if (thread->isAlive())
        Log::getDefaultLog()
            << Log::EL_ERROR << "FilterThread is still alive in restart method!"
            << Log::endl;

    // start the threads
    for (auto thread : m_filterThreads)
      bRet &= thread->start();
  }

  return bRet;
}

void Pipeline::stop() {
  if (m_bUseThreading) {
    m_pipes.back()->disable();
    m_pipes.back()->reset();

    for (auto thread : m_filterThreads)
      thread->stop();

    // make sure the final pipe is empty
    while (m_pipes.back()->pop())
      Thread::sleep();

    m_pipes.back()->enable();
  }
}

void Pipeline::reset() {
  // FILTERS
  for (auto filter : m_filters)
    filter->reset();
  for (auto filter : m_multiFilters)
    filter->reset();

  // PIPES
  for (auto pipe : m_pipes)
    pipe->reset();

  PipeData::reset();
}

std::shared_ptr<PipeData> Pipeline::step() {
  if (!m_bUseThreading) {
    for (auto thread : m_filterThreads)
      thread->run();
  }

  return m_pipes.back()->blockingPop();
}

void Pipeline::checkThreads() {
  if (m_bUseThreading) {
    for (auto thread : m_filterThreads)
      if (!thread->isFiltering())
        Log::getDefaultLog()
            << Log::EL_ERROR << "A FilterThread is not alive anymore!"
            << Log::endl;
  }
}
