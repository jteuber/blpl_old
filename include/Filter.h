#pragma once

#include "Pipeline_global.h"

#include <memory>

#include "PipeData.h"
#include "Profiler.h"

/**
 * @brief This is the interface for all filters of the pipeline. It takes an
 * input, processes it and produces an output to be processed by the next
 * filter.
 * This type of filter only takes one input PipeData.For multiple inputs looks
 * at MultiFilter
 */
class PIPELINE_EXPORT Filter
{
public:
    Filter()
        : m_prof(typeid(*this).name())
    {}

    virtual std::shared_ptr<PipeData> process(std::shared_ptr<PipeData> pIn)
    {
        m_prof.startCycle();
        auto out = processImpl(std::move(pIn));
        m_prof.endCycle();

        return out;
    }

    /**
     * @brief When implementing this interface, this is the part that's used in
     * the pipeline.
     *
     * @param pIn Shared pointer to some kind of input data of the type
     * PipeData.
     *
     * @return Shared pointer to the output of the type PipeData.
     */
    virtual std::shared_ptr<PipeData>
    processImpl(std::shared_ptr<PipeData> pIn) = 0;

    /**
     * @brief When implemented, this method should reset the filter to it's
     * original state, so that the next call of Filter::process behaves like
     * this object was just created.
     */
    virtual void reset() = 0;

private:
    Profiler m_prof;
};

/**
 * @brief This is the interface for all filters of the pipeline which take
 * multiple input PipeData and output a single PipeData instance.
 */
class PIPELINE_EXPORT MultiFilter
{
public:
    MultiFilter()
        : m_prof(typeid(*this).name())
    {}

    virtual std::shared_ptr<PipeData>
    process(std::vector<std::shared_ptr<PipeData>> pIn)
    {
        m_prof.startCycle();
        auto out = processImpl(std::move(pIn));
        m_prof.endCycle();

        return out;
    }

    /**
     * @brief When implementing this interface, this is the part that's used in
     * the pipeline.
     *
     * @param pIn Vector of shared pointers to some kind of input data of the
     * type PipeData.
     *
     * @return Shared pointer to the output of the type PipeData.
     */
    virtual std::shared_ptr<PipeData>
    processImpl(std::vector<std::shared_ptr<PipeData>> pIn) = 0;

    /**
     * @brief When implemented, this method should reset the filter to it's
     * original state, so that the next call of Filter::process behaves like
     * this object was just created.
     */
    virtual void reset() = 0;

private:
    Profiler m_prof;
};
