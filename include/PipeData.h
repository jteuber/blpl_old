#pragma once

#include "Pipeline_global.h"

#include <chrono>
#include <memory>
#include <vector>

using HighResClock = std::chrono::high_resolution_clock;
using HighResTimeStamp =
    std::chrono::time_point<std::chrono::high_resolution_clock,
                            std::chrono::duration<double>>;

class PIPELINE_EXPORT PipeData
{
public:
    PipeData()
        : dataID(++sm_lastID)
        , frameTimeStamp(HighResClock::now())
    {}

    PipeData(PipeData* prevData)
        : dataID(prevData->dataID)
        , frameTimeStamp(prevData->frameTimeStamp)
    {}

    virtual ~PipeData() {}

    const unsigned int dataID;
    HighResTimeStamp frameTimeStamp;

    static void reset() { sm_lastID = 0; }

private:
    static unsigned int sm_lastID;
};
