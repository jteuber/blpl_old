#include "blpl/Filter.h"
#include "blpl/MultiFilter.h"

#include <catch2/catch.hpp>

using namespace blpl;

class TestFilter1 : public Filter<int, float>
{
public:
    float processImpl(int&& in) override
    {
        return static_cast<float>(in) / 2.f;
    }
};

class TestFilter2 : public Filter<int, float>
{
public:
    float processImpl(int&& in) override
    {
        return static_cast<float>(in) * 2.f;
    }
};

TEST_CASE("simple multifilter construction")
{
    auto filter1     = std::make_shared<TestFilter1>();
    auto filter2     = std::make_shared<TestFilter2>();
    auto multifilter = filter1 & filter2;

    REQUIRE(multifilter.size() == 2);
}

TEST_CASE("bigger multifilter construction")
{
    auto filter1 = std::make_shared<TestFilter1>();
    auto filter2 = std::make_shared<TestFilter2>();
    auto filter3 = std::make_shared<TestFilter2>();

    auto multifilter = filter1 & filter2 & filter3;

    REQUIRE(multifilter.size() == 3);
}

TEST_CASE("simple multifilter process")
{
    auto filter1     = std::make_shared<TestFilter1>();
    auto filter2     = std::make_shared<TestFilter2>();
    auto multifilter = filter1 & filter2;

    std::vector<int> in(2, 2);
    std::vector<float> out = multifilter.process(std::move(in));

    REQUIRE(out[0] * out[1] == 4);
}
