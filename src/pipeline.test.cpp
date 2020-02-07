#include <Filter.h>
#include <Pipeline.h>

#include <catch2/catch.hpp>

class TestFilter1 : public Filter<int, float>
{
public:
    float processImpl(int&& in) override
    {
        return static_cast<float>(in) / 2.f;
    }
    void reset() override {}
};

class TestFilter2 : public Filter<float, std::string>
{
public:
    std::string processImpl(float&& in) override
    {
        return std::to_string(in);
    }
    void reset() override {}
};

TEST_CASE("Pipeline construction")
{
    auto filter1 = std::make_shared<TestFilter1>();
    auto filter2 = std::make_shared<TestFilter2>();

    CHECK(std::string(typeid(*filter1).name()).find("TestFilter1") !=
          std::string::npos);

    auto pipeline =
        std::static_pointer_cast<Filter<int, float>>(filter1) |
        std::static_pointer_cast<Filter<float, std::string>>(filter2);

    CHECK(pipeline.m_filters.size() == 2);
}
