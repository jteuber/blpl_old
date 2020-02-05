#pragma once

#include "Pipeline_global.h"

#include <list>

#include "FilterThread.h"
#include "Pipe.h"

template <class InData, class OutData>
class Pipeline
{
public:
    // private:
    std::shared_ptr<Pipe<InData>> m_inPipe;
    std::shared_ptr<Pipe<OutData>> m_outPipe;

    std::list<std::shared_ptr<AbstractFilterThread>> m_filters;
};

template <class InData, class IntermediateData, class OutData>
Pipeline<InData, OutData>
operator|(std::shared_ptr<Filter<InData, IntermediateData>> first,
          std::shared_ptr<Filter<IntermediateData, OutData>> second)
{
    Pipeline<InData, OutData> pipeline;

    // start with the pipes
    pipeline.m_inPipe  = std::make_shared<Pipe<InData>>();
    auto betweenPipe   = std::make_shared<Pipe<IntermediateData>>();
    pipeline.m_outPipe = std::make_shared<Pipe<OutData>>();

    // then create the filter threads
    pipeline.m_filters.push_back(
        std::make_shared<FilterThread<InData, IntermediateData>>(
            pipeline.m_inPipe, first, betweenPipe));
    pipeline.m_filters.push_back(
        std::make_shared<FilterThread<IntermediateData, OutData>>(
            betweenPipe, second, pipeline.m_outPipe));

    return pipeline;
}

template <class InData, class IntermediateData, class OutData>
Pipeline<InData, OutData>
operator|(Pipeline<InData, IntermediateData>&& pipeline,
          std::shared_ptr<Filter<IntermediateData, OutData>> filter)
{
    Pipeline<InData, OutData> newPipeline;
    newPipeline.m_inPipe  = std::move(pipeline.m_inPipe);
    newPipeline.m_filters = std::move(pipeline.m_filters);

    // prepare the pipes
    auto betweenPipe = pipeline.m_outPipe;
    newPipeline.m_outPipe.reset(std::make_shared<Pipe<OutData>>());

    // add the filter thread
    newPipeline.m_filters.push_back(
        std::make_shared<FilterThread<IntermediateData, OutData>>(
            betweenPipe, filter, newPipeline.m_outPipe));

    return newPipeline;
}
