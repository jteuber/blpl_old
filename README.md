# blpl

Welcome to the big, lightweight pipeline: a modern C++ pipeline library for high concurrency, high through-put 
computation on the CPU.

The concurrency is happening on a per filter-instance level. This means that one specific filter in the pipeline is
 always running sequentially, making it possible for the filter to have state, while all filters run parallel to each
  other, enabling very high concurrency on pipelines with a medium to high number of filters.

This library was originally developed for a research project using computer-vision and machine-learning. The goal was
 to have a lightweight pipeline library that adds as little overhead as possible. 

## How to compile

This library uses [dds](https://github.com/vector-of-bool/dds) as build system, but it also provides cmake files for
 convenience.

If you use dds just do `dds build -t :gcc` (or some other toolchain of your choice).

With cmake do the usual
```shell script
mkdir build && cd build && cmake .. && make && sudo make install
```
in the library directory.

The library has for now only been tested on linux. It might still have some hickups on windows and mac, but should in theory (tm) work fine.
 
## How to use

```c++
// write a filter by inheriting Filter<InData,OutData>. *Generator* is a special type that tells the pipeline to
// continuously call this filter.
class StartFilter : public blpl::Filter<Generator, int>
{
public:
    // Implement your filter in processImpl
    int processImpl(Generator&&) override
    {
        return m_i++;
    }

    // and a reset method to reset any state of the filter to its initial state
    void reset() override { m_i = 0; }
    // we don't have to worry about the state being thread safe as every instance of this filter will run sequentially
    int m_i = 0;
};

// and then a few more that do actual computation
...
// and a last one that produces and maybe stores or shows the final output
class EndFilter : public blpl::Filter<std::string, int>
{
public:
    // the payload in the pipes are moved to have as little overhead as possible. If you need shared resources just
    // use shared pointers to those resources.
    int processImpl(std::string&& in) override
    {
        std::cout << in;
        return 0;
    }
};

int main() 
{
    // instantiate the filters
    auto startFilter = std::make_shared<StartFilter>();
    auto filter1 = std::make_shared<TestFilter1>();
    ...
    auto endFilter = std::make_shared<EndFilter>();
    
    // build the pipeline
    auto pipeline = startFilter |
                    filter1 |
                    ... |
                    endFilter;
    
    // start the pipeline
    pipeline.start();

    // this is one way of waiting for the pipeline to be done, just return a status code in the last filter
    int status = 0;
    while(status == 0)
        status = pipeline.outPipe()->blockingPop();

    // stop the pipeline. Stops the pipes and waits for all the threads to finish up their last computation
    pipeline.stop();
}
```

## Poor Mans Profiler

This library also contains a profiler and a very simple logger that supports logging into several files at ones. The
profiler is usually compiled out. To use it you have to compile this library with `PROFILE` defined. Then the profiler
will write one file for every filter in the pipeline with the processing time per run taken. The name of the file will
be the symbol name of the class that defines the filter.

## License

This library uses the MIT license:

Copyright 2020 Jörn Teuber

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
  OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
   OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
