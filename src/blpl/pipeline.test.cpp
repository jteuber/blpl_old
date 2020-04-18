#include "blpl/Filter.h"
#include "blpl/Pipeline.h"

#include <catch2/catch.hpp>

using namespace blpl;

class TestFilter0 : public Filter<Generator, int>
{
public:
    int processImpl(Generator&&) override
    {
        if (m_i < 100)
            return m_i++;
        return m_i;
    }

    void reset() override
    {
        m_i = 0;
    }

    int m_i = 0;
};

class TestFilter1 : public Filter<int, float>
{
public:
    float processImpl(int&& in) override
    {
        return static_cast<float>(in) / 2.f;
    }
};

class TestFilter2 : public Filter<float, std::string>
{
public:
    std::string processImpl(float&& in) override
    {
        return std::to_string(in);
    }
};

class TestFilter3 : public Filter<std::string, std::string>
{
public:
    std::string processImpl(std::string&& in) override
    {
        m_lastInput = in;
        return std::move(in);
    }

    std::string m_lastInput;
};

TEST_CASE("simple pipeline construction")
{
    auto filter1  = FilterPtr<int, float>(new TestFilter1);
    auto filter2  = FilterPtr<float, std::string>(new TestFilter2);
    auto pipeline = filter1 | filter2;

    REQUIRE(pipeline.length() == 2);
}

TEST_CASE("bigger pipeline construction")
{
    auto filter1 = std::make_shared<TestFilter1>();
    auto filter2 = std::make_shared<TestFilter2>();
    auto filter3 = std::make_shared<TestFilter3>();

    auto pipeline = filter1 | filter2 | filter3;

    REQUIRE(pipeline.length() == 3);
}

TEST_CASE("pipeline test")
{
    auto filter0  = std::make_shared<TestFilter0>();
    auto filter1  = std::make_shared<TestFilter1>();
    auto filter2  = std::make_shared<TestFilter2>();
    auto filter3  = std::make_shared<TestFilter3>();
    auto pipeline = filter0 | filter1 | filter2 | filter3;

    REQUIRE(pipeline.length() == 4);

    pipeline.start();
    int output = 0;
    while (output++ < 100)
        pipeline.outPipe()->blockingPop();
    pipeline.stop();

    REQUIRE(filter0->m_i == 100);
    REQUIRE(std::stoi(filter3->m_lastInput) == 50);
}
