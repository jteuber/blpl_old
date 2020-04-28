#pragma once

#include <cassert>
#include <thread>
#include <vector>

#include "Filter.h"

namespace blpl {

/**
 * @brief This is the interface for all filters of the pipeline. It takes an
 * input, processes it and produces an output to be processed by the next
 * filter.
 */
template <class InData, class OutData>
class PIPELINE_EXPORT MultiFilter
    : public Filter<std::vector<InData>, std::vector<OutData>>
{
public:
    template <class Filter1, class Filter2>
    MultiFilter(std::shared_ptr<Filter1> first,
                std::shared_ptr<Filter2> second);

    template <class FilterClass>
    MultiFilter(std::vector<std::shared_ptr<FilterClass>> filterVector);

    template <class ExtendingFilter>
    MultiFilter<InData, OutData>&
    operator&(std::shared_ptr<ExtendingFilter> filter);

    [[nodiscard]] size_t size() const;

    std::vector<OutData> processImpl(std::vector<InData>&& in) override
    {
        assert(!m_filters.empty());

        // call process of all sub-filters in their own thread
        std::vector<std::thread> threads;
        std::vector<OutData> out(m_filters.size());
        for (size_t i = 1; i < m_filters.size(); ++i)
            threads.emplace_back(
                [&, index = i, in = std::move(in[i])]() mutable {
                    out[index] = m_filters[index]->processImpl(std::move(in));
                });

        // but execute the first filter in this thread
        out[0] = m_filters[0]->processImpl(std::move(in[0]));

        for (auto& thread : threads)
            thread.join();

        return out;
    }

private:
    std::vector<FilterPtr<InData, OutData>> m_filters;
};

template <class InData, class OutData>
template <class Filter1, class Filter2>
MultiFilter<InData, OutData>::MultiFilter(std::shared_ptr<Filter1> first,
                                          std::shared_ptr<Filter2> second)
{
    static_assert(
        std::is_base_of<Filter<InData, OutData>, Filter1>::value,
        "First filter does not inherit from the correct Filter template");
    static_assert(
        std::is_base_of<Filter<InData, OutData>, Filter2>::value,
        "Second filter does not inherit from the correct Filter template");

    m_filters.push_back(first);
    m_filters.push_back(second);
}

template <class InData, class OutData>
template <class FilterClass>
MultiFilter<InData, OutData>::MultiFilter(
    std::vector<std::shared_ptr<FilterClass>> filterVector)
{
    static_assert(
        std::is_base_of<Filter<InData, OutData>, FilterClass>::value,
        "Given filters do not inherit from the correct Filter template");

    // just copy the vector
    for (auto& filter : filterVector) {
        m_filters.push_back(filter);
    }
}

template <class InData, class OutData>
template <class ExtendingFilter>
MultiFilter<InData, OutData>&
MultiFilter<InData, OutData>::operator&(std::shared_ptr<ExtendingFilter> filter)
{
    static_assert(
        std::is_base_of<Filter<InData, OutData>, ExtendingFilter>::value,
        "Extending filter does not inherit from the correct Filter template");

    m_filters.push_back(filter);

    return *this;
}

template <class Filter1, class Filter2>
PIPELINE_EXPORT MultiFilter<typename Filter1::inType, typename Filter1::outType>
operator&(std::shared_ptr<Filter1> first, std::shared_ptr<Filter2> second)
{
    return MultiFilter<typename Filter1::inType, typename Filter1::outType>(
        first, second);
}

template <class InData, class OutData>
size_t MultiFilter<InData, OutData>::size() const
{
    return m_filters.size();
}

} // namespace blpl
