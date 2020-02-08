#include <Filter.h>
#include <Pipeline.h>

#include <catch2/catch.hpp>

template <class InData, class OutData>
using FilterPtr = std::shared_ptr<Filter<InData, OutData>>;

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

class TestFilter3 : public Filter<std::string, std::string>
{
public:
    std::string processImpl(std::string&& in) override
    {
        return std::move(in);
    }

    void reset() override {}
};

TEST_CASE("simple pipeline construction")
{
    auto filter1 = std::shared_ptr<Filter<int, float>>(new TestFilter1);
    auto filter2 = std::shared_ptr<Filter<float, std::string>>(new TestFilter2);
    auto pipeline = filter1 | filter2;

    REQUIRE(pipeline.length() == 2);
}

TEST_CASE("bigger pipeline construction")
{
    auto filter1 = std::make_shared<TestFilter1>();
    auto filter2 = std::make_shared<TestFilter2>();
    auto filter3 = std::make_shared<TestFilter3>();

    auto pipeline =
        std::static_pointer_cast<Filter<int, float>>(filter1) |
        std::static_pointer_cast<Filter<float, std::string>>(filter2) |
        std::static_pointer_cast<Filter<std::string, std::string>>(filter3);

    REQUIRE(pipeline.length() == 3);
}

TEST_CASE("simpler pipeline construction")
{
    auto filter1  = FilterPtr<int, float>(new TestFilter1);
    auto filter2  = FilterPtr<float, std::string>(new TestFilter2);
    auto filter3  = FilterPtr<std::string, std::string>(new TestFilter3);
    auto pipeline = filter1 | filter2 | filter3;

    REQUIRE(pipeline.length() == 3);
}
