#pragma once

#include "Pipeline_global.h"

#include <list>

#include "AbstractPipeline.h"
#include "FilterThread.h"
#include "Pipe.h"

/// GENERAL CLASS CASE
template <class InData, class OutData>
class Pipeline : public AbstractPipeline {
public:
    template<class IntermediateData>
    Pipeline(std::shared_ptr<Filter<InData, IntermediateData>> first,
             std::shared_ptr<Filter<IntermediateData, OutData>> second);

    template<class ExtendedData>
    Pipeline<InData, ExtendedData>
    operator|(std::shared_ptr<Filter<OutData, ExtendedData>> filter);

private:
    explicit Pipeline() = default;

private:
    std::shared_ptr<Pipe<InData>> m_inPipe;
    std::shared_ptr<Pipe<OutData>> m_outPipe;
};

template<class InData, class OutData>
template<class ExtendedData>
Pipeline<InData, ExtendedData> Pipeline<InData, OutData>::operator|(
    std::shared_ptr<Filter<OutData, ExtendedData>> filter) {
    Pipeline<InData, ExtendedData> newPipeline;
    newPipeline.m_inPipe = std::move(m_inPipe);
    newPipeline.m_filters = std::move(m_filters);

    // prepare the pipes
    auto betweenPipe = m_outPipe;
    newPipeline.m_outPipe = std::make_shared<Pipe<OutData>>();

    // add the filter thread
    newPipeline.m_filters.push_back(
        std::make_shared<FilterThread<OutData, ExtendedData>>(
            betweenPipe, filter, newPipeline.m_outPipe));

    return newPipeline;
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
