#ifndef PROFILER_H
#define PROFILER_H

#include <string>
#include <chrono>

#include "Log.h"

class Profiler
{
public:
    Profiler( std::string className );

    void startCycle();
    void endCycle();

private:
    std::string m_className;
    Log& m_log;

    std::chrono::high_resolution_clock::time_point m_tStart;
};

#endif // PROFILER_H
