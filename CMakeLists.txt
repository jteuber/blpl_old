cmake_minimum_required(VERSION 3.5)

project(Pipeline LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(SOURCES
    src/Log.cpp
    src/Profiler.cpp
)

set(HEADERS
    include/Filter.h
    include/FilterThread.h
    include/Log.h
    include/Pipe.h
    include/Pipeline.h
    include/Pipeline_global.h
    include/Profiler.h
)

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

target_compile_definitions(Pipeline PRIVATE PIPELINE_LIBRARY)
