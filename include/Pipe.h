#pragma once

#include "Pipeline_global.h"

#include <memory>
#include <mutex>
#include <queue>

#include "PipeData.h"
#include "Thread.h"

class PIPELINE_EXPORT Pipe {
public:
  Pipe(bool waitForSlowestFilter = false, bool bBuffered = false);
  virtual ~Pipe();

  virtual std::shared_ptr<PipeData> pop();
  virtual std::shared_ptr<PipeData> blockingPop();
  virtual void push(const std::shared_ptr<PipeData> &pData);
  virtual void reset();

  virtual void disable();
  virtual void enable();

  virtual unsigned int size();

private:
  std::shared_ptr<PipeData> m_spElem;
  std::mutex m_mutex;

  bool m_bBuffered;
  bool m_waitForSlowestFilter;

  bool m_bEnabled;
};

class PIPELINE_EXPORT NullPipe : public Pipe {
public:
  NullPipe(int msecsBetweenPops = 0);
  virtual ~NullPipe() {}

  virtual std::shared_ptr<PipeData> pop();
  virtual std::shared_ptr<PipeData> blockingPop();
  virtual void push(const std::shared_ptr<PipeData> & /*pData*/) {}
  virtual void reset() {}

  virtual void disable() {}
  virtual void enable() {}

  virtual unsigned int size() { return 1; }

private:
  bool m_wait;
  int m_msecsBetweenPops;

  std::chrono::high_resolution_clock::time_point m_lastPop;
};
