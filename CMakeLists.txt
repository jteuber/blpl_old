cmake_minimum_required(VERSION 3.9)

project(blpl VERSION 0.1.0 DESCRIPTION "big, lightweight pipeline" LANGUAGES CXX)

include(GNUInstallDirs)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    src/blpl/Log.cpp
    src/blpl/Profiler.cpp
    src/blpl/AbstractPipeline.cpp
    src/blpl/AbstractPipe.cpp)

set(HEADERS
    include/blpl/Filter.h
    include/blpl/FilterThread.h
    include/blpl/MultiFilter.h
    include/blpl/Log.h
    include/blpl/Generator.h
    include/blpl/Pipe.h
    include/blpl/Pipeline.h
    include/blpl/Pipeline_global.h
    include/blpl/Profiler.h
    include/blpl/AbstractPipeline.h
    include/blpl/AbstractPipe.h)

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

set_target_properties(${PROJECT_NAME} PROPERTIES 
    VERSION ${PROJECT_VERSION}
    SOVERSION 0
    PUBLIC_HEADER "${HEADERS}")

target_include_directories(${PROJECT_NAME} PRIVATE include)
target_compile_definitions(${PROJECT_NAME} PRIVATE PIPELINE_LIBRARY)

install(
    TARGETS ${PROJECT_NAME} EXPORT blpl
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

#configure_file(blpl.pc.in blpl.pc @ONLY)
#install(FILES ${CMAKE_BINARY_DIR}/blpl.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)

install(EXPORT blpl DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/blpl)
#export(TARGETS ${PROJECT_NAME} FILE blplConfig.cmake)



# TESTS

find_package(Catch2 REQUIRED)
include(CTest)
include(Catch)


add_executable(testFilterThread src/blpl/filterthread.test.cpp)
target_link_libraries(testFilterThread Catch2::Catch2 blpl pthread)
target_include_directories(testFilterThread PRIVATE include)
target_compile_definitions(testFilterThread PRIVATE CATCH_CONFIG_MAIN)
catch_discover_tests(testFilterThread)

add_executable(testMultiFilter src/blpl/multifilter.test.cpp)
target_link_libraries(testMultiFilter Catch2::Catch2 blpl pthread)
target_include_directories(testMultiFilter PRIVATE include)
target_compile_definitions(testMultiFilter PRIVATE CATCH_CONFIG_MAIN)
catch_discover_tests(testMultiFilter)

add_executable(testPipeline src/blpl/pipeline.test.cpp)
target_link_libraries(testPipeline Catch2::Catch2 blpl pthread)
target_include_directories(testPipeline PRIVATE include)
target_compile_definitions(testPipeline PRIVATE CATCH_CONFIG_MAIN)
catch_discover_tests(testPipeline)

add_executable(testPipe src/blpl/pipe.test.cpp)
target_link_libraries(testPipe Catch2::Catch2 blpl pthread)
target_include_directories(testPipe PRIVATE include)
target_compile_definitions(testPipe PRIVATE CATCH_CONFIG_MAIN)
catch_discover_tests(testPipe)
