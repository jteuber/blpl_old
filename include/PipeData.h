#pragma once

#include "Pipeline_global.h"

#include <chrono>
#include <memory>
#include <vector>

typedef std::chrono::time_point<std::chrono::high_resolution_clock,
                                std::chrono::duration<double>>
    highResTimeStamp;

class PIPELINE_EXPORT PipeData
{
private:
    friend class Pipeline;
    static unsigned int sm_lastID;

public:
    PipeData()
        : dataID(++sm_lastID)
        , frameTimeStamp(std::chrono::high_resolution_clock::now())
    {}

    PipeData(PipeData* prevData)
        : dataID(prevData->dataID)
        , frameTimeStamp(prevData->frameTimeStamp)
    {}

    virtual ~PipeData() {}

    const unsigned int dataID;
    highResTimeStamp frameTimeStamp;
};
