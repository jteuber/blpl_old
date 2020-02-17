#pragma once

#include "Pipeline_global.h"

#include <chrono>
#include <thread>

namespace blpl {

using HighResClock = std::chrono::high_resolution_clock;
using HighResTimeStamp =
    std::chrono::time_point<std::chrono::high_resolution_clock,
                            std::chrono::duration<double>>;

class PIPELINE_EXPORT Generator
{
public:
    explicit Generator(HighResTimeStamp finishAt)
    {
        std::this_thread::sleep_until(finishAt);
    }
    Generator() = default;
};

}
