=========================================
Semi-columnar-array
=========================================
**High-performance Columnar Data Engine for Semiconductor Characterization**

:Author: Ying-Ying Tso (312651030)
:Status: Week 1 - Core Architecture & Memory Alignment (Completed)
:GitHub: https://github.com/qqsc12/semi-columnar-array

.. contents:: Table of Contents
   :depth: 2

Problem to Solve
================
Semiconductor electrical characterization (e.g., from Keysight B1500) generates massive datasets with dynamic schemas. Using a traditional **Array of Structs (AoS)** layout causes each row to occupy a full **64-byte Cache Line**, leading to poor cache performance when analyzing single parameters across many samples.

This project implements a **Columnar Array (Struct of Arrays, SoA)** library to provide:

* **Superior Cache Locality**: Sequential memory access for individual measurement parameters.
* **SIMD-ready Layout**: 64-byte aligned memory buffers to support AVX/vectorized math.
* **Zero-copy Integration**: Shared memory access between C++ and Python via ``pybind11``.

Technical Schedule & Testing Plan
=================================

We strictly follow the **"Test-as-you-code"** principle. Each milestone includes a specific verification task.

+----------+-------------------+----------------------------------------------------------+-------------------------------------------------+
| Phase    | Duration          | Technical Focus                                          | Test Task                                       |
+==========+===================+==========================================================+=================================================+
| **Week 1**| Apr 26 - May 02 | **Core & Alignment**: CMake setup and ``ColumnarArray`` | Validate 64-byte alignment in                   |
|          |                   | template using ``std::aligned_alloc``.                   | ``tests/test_kernel.cpp``.                      |
+----------+-------------------+----------------------------------------------------------+-------------------------------------------------+
| **Week 2**| May 03 - May 09 | **Data Ingestion**: B1500 CSV parser and math kernels    | Use ``assert`` to verify numerical accuracy of  |
|          |                   | (mean, variance). Implement **Row-accessor** handle.     | the math kernels.                               |
+----------+-------------------+----------------------------------------------------------+-------------------------------------------------+
| **Week 3**| May 10 - May 16 | **Python Bridge**: ``pybind11`` integration for the      | Implement ``pytest`` suite to ensure            |
|          |                   | Python wrapper.                                          | **zero-copy** data integrity.                   |
+----------+-------------------+----------------------------------------------------------+-------------------------------------------------+
| **Week 4**| May 17 - May 23 | **Robustness**: Handling "Dirty CSV" with irregular      | Unit tests for irregular B1500 headers and      |
|          |                   | headers. System architecture refinement.                 | missing data points.                            |
+----------+-------------------+----------------------------------------------------------+-------------------------------------------------+
| **Week 5**| May 24 - May 30 | **Benchmarking**: Performance profiling and final        | Conduct benchmarks comparing SoA (C++) against  |
|          |                   | documentation.                                           | standard row-based Pandas access.               |
+----------+-------------------+----------------------------------------------------------+-------------------------------------------------+

Current Progress (Week 1)
=========================
* [x] **Project Skeleton**: Initialized CMake, ``include/``, and ``tests/`` directories.
* [x] **Memory Management**: Implemented ``ColumnarArray`` template with Unit 7 alignment techniques.
* [x] **Verification**: ``test_kernel.cpp`` confirmed the heap memory is strictly **64-byte aligned** (Address % 64 == 0).

Build and Test Instructions
===========================
To verify the current Week 1 progress, run the following commands:

.. code-block:: bash

   # 1. Build the project
   mkdir -p build && cd build
   cmake .. && make

   # 2. Run the memory alignment test
   ./test_kernel

References
==========
* **Apache Arrow Columnar Format**
* **NSD Unit 7: Memory Management & Alignment**