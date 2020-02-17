#pragma once

#include "Pipeline_global.h"

#include <memory>

#include "PipeData.h"
#include "Profiler.h"

namespace blpl {

class PIPELINE_EXPORT AbstractFilter
{
    /**
     * @brief When implemented, this method should reset the filter to it's
     * original state, so that the next call of Filter::process behaves like
     * this object was just created.
     */
    virtual void reset() {};
};

/**
 * @brief This is the interface for all filters of the pipeline. It takes an
 * input, processes it and produces an output to be processed by the next
 * filter.
 */
template <class InData, class OutData>
class PIPELINE_EXPORT Filter : public AbstractFilter
{
public:
    /**
     * @brief When implementing this interface, this is the part that's used in
     * the pipeline.
     *
     * @param in Shared pointer to some kind of input data of the type
     * PipeData.
     *
     * @return Shared pointer to the output of the type PipeData.
     */
    virtual OutData processImpl(InData&& in) = 0;

    Filter()
        : m_prof(typeid(*this).name())
    {}

    virtual OutData process(InData&& pIn)
    {
        m_prof.startCycle();
        auto out = processImpl(std::move(pIn));
        m_prof.endCycle();

        return out;
    }

private:
    Profiler m_prof;
};

/// Convenience type for shared_ptr to filters
template <class InData, class OutData>
using FilterPtr = std::shared_ptr<Filter<InData, OutData>>;

}
