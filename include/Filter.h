#pragma once

#include "Pipeline_global.h"

#include <memory>

#include "PipeData.h"
#include "Profiler.h"

/**
 * @brief This is the interface for all filters of the pipeline, i.e. taking an
 * input, processing it and producing an output to be processed by the next
 * filter, which take only one input PipeData.
 */
class PIPELINE_EXPORT Filter {
public:
  Filter() : m_prof(nullptr) {}
  virtual ~Filter() {}

  virtual std::shared_ptr<PipeData> process(std::shared_ptr<PipeData> pIn) {
#ifdef PROFILE
    if (!m_prof)
      m_prof = new Profiler(typeid(*this).name());

    m_prof->startCycle();
#endif
    auto out = processImpl(pIn);

#ifdef PROFILE
    m_prof->endCycle();
#endif

    return out;
  }

  /**
   * @brief When implementing this interface, this is the part that's used in
   * the pipeline.
   *
   * @param pIn Smart pointer to some kind of input data of the type PipeData.
   *
   * @return Smart pointer to the output of the type PipeData.
   */
  virtual std::shared_ptr<PipeData>
  processImpl(std::shared_ptr<PipeData> pIn) = 0;

  /**
   * @brief When implemented, this method should reset the filter to it's
   * original state, so that the next call of Filter::process behaves like this
   * object was just created.
   */
  virtual void reset() = 0;

private:
  Profiler *m_prof;
};

/**
 * @brief This is the interface for all filters of the pipeline which take
 * multiple input PipeData and output a single PipeData instance.
 */
class PIPELINE_EXPORT MultiFilter {
public:
  MultiFilter() {}
  virtual ~MultiFilter() {}

  virtual std::shared_ptr<PipeData>
  process(std::vector<std::shared_ptr<PipeData>> pIn) {
#ifdef PROFILE
    if (!m_prof)
      m_prof = new Profiler(typeid(*this).name());

    m_prof->startCycle();
#endif
    auto out = processImpl(pIn);

#ifdef PROFILE
    m_prof->endCycle();
#endif

    return out;
  }

  /**
   * @brief When implementing this interface, this is the part that's used in
   * the pipeline.
   *
   * @param pIn Vector of smart pointers to some kind of input data of the type
   * PipeData.
   *
   * @return Smart pointer to the output of the type PipeData.
   */
  virtual std::shared_ptr<PipeData>
  processImpl(std::vector<std::shared_ptr<PipeData>> pIn) = 0;

  /**
   * @brief When implemented, this method should reset the filter to it's
   * original state, so that the next call of Filter::process behaves like this
   * object was just created.
   */
  virtual void reset() = 0;

private:
  Profiler *m_prof;
};
