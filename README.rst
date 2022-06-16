Overview
---------

This is a prototype, work-in-progress library for gigantic, file-backed,
thread-safe, bit-resolution arrays. The goal of this library is to provide
useful abstractions and reasonable performance for tools that want to process
arbitrarily large blobs of data, like
`hobbits <https://github.com/Mahlet-Inc/hobbits>`_.

Building
*********

Requirements:

* CMake 3.16+
* A C++ compiler that supports C++17 (e.g. GCC 9+)
* (optional) Ninja

Clone and build the project:
::

    git clone --recurse-submodules https://github.com/smr-llc/bitray.git
    cd bitray
    cmake . -Bbuild -GNinja -DCMAKE_BUILD_TYPE=Release -DBITRAY_BUILD_TESTS=ON
    cmake --build build

Run a benchmark:
::

    ./build/tests/benchmarks/bm_byteray

Building the Documentation
***************************

Building the documentation requires:

* Python
* Doxygen
* Sphinx, Breathe, and sphinx_rtd_theme

Some of the requirements can be installed with ``pip``:
::

    pip install sphinx sphinx_rtd_theme breathe

Build the docs:
::

    cmake . -Bbuild -GNinja -DBITRAY_BUILD_DOCS=ON
    cmake --build build