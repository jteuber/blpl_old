#pragma once

#include "Pipeline_global.h"

#include "FilterThread.h"
#include "Pipe.h"

class PIPELINE_EXPORT Pipeline {
public:
  Pipeline(bool waitForSlowestFilter, bool useThreading,
           int msecsBetweenFrames);
  ~Pipeline();

  void addParallelFilters(std::vector<Filter *> &filters);
  void addMultiFilter(MultiFilter *pFilter);
  void addFilter(Filter *pFilter);

  bool start();
  void stop();
  void reset();

  std::shared_ptr<PipeData> step();

  void checkThreads();

private:
  std::vector<Filter *> m_filters;
  std::vector<MultiFilter *> m_multiFilters;
  std::vector<Pipe *> m_pipes;
  std::vector<FilterThread *> m_filterThreads;

  unsigned int m_lastPipeID;
  bool m_waitForSlowestFilter;
  bool m_bUseThreading;
  int m_msecsBetweenFrames;
  unsigned int m_nrOfSources;
};
