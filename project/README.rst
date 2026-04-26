<<<<<<< HEAD
================
NSD Term Project
================

This is the directory for the term project proposal.  To turn it in, you need
**a PR** and **an E3 entry for the PR URL**.  See
https://yyc.solvcon.net/en/latest/nsd/schedule/26sp_nycu/schedule26sp.html for
what should be included in the proposal and project.

The proposal accounts for 8 points out of the 40 points allocated to the term
project.  Like homework, you will need to create a PR against the ``project``
branch (not ``master``).  You need to create a sub-directory using exactly your
GitHub username as your working directory (``nsdhw_26sp/project/$username/``).
The hierarchy should be like::

  - nsdhw_26sp (<- repository root)
    - project
      - username (<- your working directory)
        - README.rst (<- the main proposal file)
        - ... other files for your proposal

**ATTENTION**: Your PR should include an entry in "Project List by Account
Name" in this file.

When submitting your proposal, name the PR as ``<username>-proposal**``, e.g.,
``yungyuc-proposal-submission``.  Don't forget to put the PR URL in the E3
entry too.  You can request my review in the PR.

Presentation
============

..
  The presentation schedule is set.  If you want to change the time, ask the
  presenter at the other time slot, and file a PR tagging him or her and the
  instructor (@yungyuc) against the branch `master`.  Everyone involved needs to
  leave a global comment to agree the exchange in the PR.  The PR subject line
  should start with ``[presentation]``.

  Each presenter has at most 15 minutes including setup.  A common arrangement is
  to present for 12 minutes and use 2 minutes for questions and discussions.

  Presenters should prepare the computer.  It is OK to share.  If a presenter needs
  help to prepare a computer, they need to bring it up and get it resolved one week
  (168 hours) before the presentation.

Projects by Account Names
+++++++++++++++++++++++++


Follow the format to add your project:

0. `github_account_name <https://github.com/github_account_name>`__ for
   `Project subject (this is an example entry) <github_account_name/README.rst>`__:
   https://github.com/github_account_name/project_name

.. note::

  Append your project after the first example entry.  Do not delete the example
  entry.

=======
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
>>>>>>> 0cf9faf9872adb830a11d2663b57d1f717082cd0
