# $1 Unistroke Recognizer

This repo implements [$1 Unistroke Recognizer](https://depts.washington.edu/acelab/proj/dollar/index.html), a fast and simple 1-stroke gesture recognizer. As demonstrated in the linked image, it could identify various shapes with 10 samples of each. Adding a new gesture also doesn't require full recomputation:

![sample](https://depts.washington.edu/acelab/proj/dollar/unistrokes.gif)

This library implemented the Protractor optimization version of the original algorithm. The official gesture logs are included as part of the test data.

## Usage

This library requires C++17 or higher C++ compiler. 

If you use CMake, it's recommended to use [CPM](https://github.com/cpm-cmake/CPM.cmake) to add this project as a dependency:

```
# Replace 0.1 with the latest git vX tag
CPMAddPackage("gh:htfy96/dollar#0.1")

target_link_libraries(your-program dollar)
```

Alternatively, you can copy all files under `include` and `src` and manually compile them as a library.

### Interface
```cpp
#include "dollar.hpp"
dollar::Stroke stroke1{{{0., 0.}, {0., 1.}, {1., 1.}}, dollar::Orientation::Sensitive};
dollar::Stroke stroke2{{{0., 0.}, {1., 1.}, {1., 0.}}, dollar::Orientation::Sensitive};
vector<dollar::Stroke> samples{stroke1, stroke2};
dollar::Stroke testStroke{{{0., 0.}, {0., 0.9}, {0.02, 0.91}, {1., 1.}}, dollar::Orientation::Sensitive};
auto [it, score] = dollar::recognize(testStroke, samples.begin(), samples.end());
assert(it == samples.begin()); // returns stroke1
```

## Comparisons with other implementations

- https://github.com/olwal/microdollar : microdollar provided a C-alike interface, while this project provides a modern interface

## LICENSE

Apache 2.0

## Credit

> - Jacob O. Wobbrock, University of Washington
> - Andrew D. Wilson, Microsoft Research
> - Yang Li, University of Washington†

