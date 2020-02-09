#pragma once

#include <list>

#include "FilterThread.h"

class AbstractPipeline {
public:
    void start();
    void stop();

    [[nodiscard]] size_t length() const;

protected:
    std::list<std::shared_ptr<AbstractFilterThread>> m_filters;
};
