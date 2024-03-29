#include "blpl/Pipe.h"

#include <catch2/catch.hpp>

#include <chrono> // std::chrono::seconds
#include <thread> // std::this_thread::sleep_for

using namespace blpl;

TEST_CASE("construction")
{
    Pipe<int> pipe;
    REQUIRE(pipe.size() == 0);
}

TEST_CASE("push")
{
    Pipe<int> pipe;
    pipe.push(1);
    REQUIRE(pipe.size() == 1);
}

TEST_CASE("disabled push")
{
    Pipe<int> pipe;
    pipe.disable();
    pipe.push(1);
    REQUIRE(pipe.size() == 0);
}

TEST_CASE("disabled enable")
{
    Pipe<int> pipe;
    pipe.disable();
    pipe.enable();
    pipe.push(1);
    REQUIRE(pipe.size() == 1);
}

TEST_CASE("pop")
{
    Pipe<int> pipe;
    pipe.push(1);
    int data = pipe.pop();
    REQUIRE(pipe.size() == 0);
    REQUIRE(data == 1);
}

TEST_CASE("push push pop default")
{
    Pipe<int> pipe;
    pipe.push(1);
    REQUIRE(pipe.size() == 1);
    pipe.push(2);
    REQUIRE(pipe.size() == 1);
    REQUIRE(pipe.pop() == 2);
}

TEST_CASE("reset")
{
    Pipe<int> pipe;
    pipe.push(1);
    REQUIRE(pipe.size() == 1);
    pipe.reset();
    REQUIRE(pipe.size() == 0);
}

TEST_CASE("blocking pop")
{
    Pipe<int> pipe;
    int data = 0;
    std::atomic<bool> threadActive(false);
    std::thread thread([&pipe, &data, &threadActive]() {
        threadActive = true;
        data         = pipe.blockingPop();
        threadActive = false;
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    REQUIRE(threadActive);
    pipe.push(1);
    thread.join();
    REQUIRE_FALSE(threadActive);
    REQUIRE(data == 1);
    REQUIRE(pipe.size() == 0);
}

TEST_CASE("blocking push")
{
    Pipe<int> pipe(true);
    pipe.push(1);

    std::atomic<bool> threadActive(false);
    std::thread thread([&pipe, &threadActive]() {
        threadActive = true;
        pipe.push(2);
        threadActive = false;
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    REQUIRE(threadActive);
    REQUIRE(pipe.pop() == 1);
    thread.join();
    REQUIRE_FALSE(threadActive);
    REQUIRE(pipe.pop() == 2);
    REQUIRE(pipe.size() == 0);
}
