#pragma once

#include "Pipeline_global.h"

#include <list>

#include "AbstractPipeline.h"
#include "FilterThread.h"
#include "Pipe.h"

/**
 * Main pipeline class.
 * @tparam InData The datatype that goes into the pipeline.
 * @tparam OutData The datatype that is produced by the pipeline.
 */
template <class InData, class OutData>
class Pipeline : public AbstractPipeline {
public:
    template<class IntermediateData>
    Pipeline(std::shared_ptr<Filter<InData, IntermediateData>> first,
             std::shared_ptr<Filter<IntermediateData, OutData>> second);

    template <class IntermediateData>
    Pipeline(Pipeline<InData, IntermediateData>&& pipeline,
             std::shared_ptr<Filter<IntermediateData, OutData>> extender);

    template<class ExtendedData>
    Pipeline<InData, ExtendedData>
    operator|(std::shared_ptr<Filter<OutData, ExtendedData>> filter);

    std::shared_ptr<Pipe<OutData>> outPipe()
    {
        return m_outPipe;
    }

private:
    explicit Pipeline() = default;

private:
    std::shared_ptr<Pipe<InData>> m_inPipe;
    std::shared_ptr<Pipe<OutData>> m_outPipe;

    template <class, class>
    friend class Pipeline;
};

template<class InData, class OutData>
template<class ExtendedData>
Pipeline<InData, ExtendedData> Pipeline<InData, OutData>::operator|(
    std::shared_ptr<Filter<OutData, ExtendedData>> filter)
{
    return Pipeline<InData, ExtendedData>(std::move(*this), filter);
}

template <class InData, class OutData>
template <class IntermediateData>
Pipeline<InData, OutData>::Pipeline(
    Pipeline<InData, IntermediateData>&& pipeline,
    std::shared_ptr<Filter<IntermediateData, OutData>> extender)
{
    m_inPipe  = std::move(pipeline.m_inPipe);
    m_filters = std::move(pipeline.m_filters);

    // prepare the pipes
    auto betweenPipe = std::move(pipeline.m_outPipe);
    m_outPipe = std::make_shared<Pipe<OutData>>();

    // add the filter thread
    m_filters.push_back(
        std::make_shared<FilterThread<IntermediateData, OutData>>(
            betweenPipe, extender, m_outPipe));
}

template<class InData, class OutData>
template<class IntermediateData>
Pipeline<InData, OutData>::Pipeline(
    std::shared_ptr<Filter<InData, IntermediateData>> first,
    std::shared_ptr<Filter<IntermediateData, OutData>> second) {
    // start with the pipes
    m_inPipe = std::make_shared<Pipe<InData>>();
    auto betweenPipe = std::make_shared<Pipe<IntermediateData>>();
    m_outPipe = std::make_shared<Pipe<OutData>>();

    // then create the filter threads
    m_filters.push_back(
        std::make_shared<FilterThread<InData, IntermediateData>>(
            m_inPipe, first, betweenPipe));
    m_filters.push_back(
        std::make_shared<FilterThread<IntermediateData, OutData>>(
            betweenPipe, second, m_outPipe));
}

template <class InData, class IntermediateData, class OutData>
Pipeline<InData, OutData>
operator|(std::shared_ptr<Filter<InData, IntermediateData>> first,
          std::shared_ptr<Filter<IntermediateData, OutData>> second)
{
    return Pipeline<InData, OutData>(first, second);
}
