#pragma once

#include "Pipeline_global.h"

#include <mutex>

#include "Filter.h"
#include "Pipe.h"
#include "Thread.h"

/**
 * @brief The FilterThread class encapsules a filter and manages it to work on
 * all incoming data in a seperate thread.
 *
 * One instance of this class only uses one thread at a time, so each filter
 * works on its incoming data sequentially. By using this on all filters of the
 * pipeline, the pipeline can work on all stages in parallel while the
 * individual stages don't have to be threadsafe (which is oftentimes not even
 * possible).
 */
class PIPELINE_EXPORT FilterThread : public Thread {
public:
    explicit FilterThread(Pipe* pOutput,
                          Filter* pFilter,
                          Pipe* pInput,
                          bool bSelfManaged = true);
    ~FilterThread() override;

    void run() override;
    virtual bool isFiltering();

    virtual void stop();

protected:
  Pipe *m_pOutput;

  volatile bool m_bFilterThreadActive;
  bool m_bSelfManaged;

private:
  Filter *m_pFilter;
  Pipe *m_pInput;
};

/**
 * @brief The FilterThread class encapsules a filter and manages it to work on
 * all incoming data in a seperate thread.
 *
 * One instance of this class only uses one thread at a time, so each filter
 * works on its incoming data sequentially. By using this on all filters of the
 * pipeline, the pipeline can work on all stages in parallel while the
 * individual stages don't have to be threadsafe (which is oftentimes not even
 * possible).
 */
class PIPELINE_EXPORT MultiFilterThread : public FilterThread
{
public:
    explicit MultiFilterThread(Pipe* pOutput,
                               MultiFilter* pMultiFilter,
                               std::vector<Pipe*>::iterator start,
                               std::vector<Pipe*>::iterator end,
                               bool bSelfManaged = true);
    ~MultiFilterThread() override;

    void run() override;
    void stop() override;

private:
    MultiFilter* m_pFilter;
    std::vector<Pipe*> m_inputPipes;
};
