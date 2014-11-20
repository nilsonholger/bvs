TODO
====

bvs
---

* GUI!!!
* binary data dump: record/replay generic data dumps (no need to write a recorder/reader for all kinds of input data)
* stats: add option to show stats (fps, timings) during run and when shutting down
* hotswap: with external struct
* logger: lock free logging in memory! and level 0 quits -> ERROR
* control: when connecting modules, change error messages to be easier to identify
* control: add mapping of thread/pool to cpu (pthread_setaffinity_np)

code
----

* compact ALL objects (objects should contain DATA only), create *mostly* pure functions, make them friend functions if they need to modify private data

testing
-------

* add testing directory 'lib/t/' with scripts and *.cc files
* create (regression) test suite using cmake CTEST
* improve code quality with gcov and valgrind

config
------

* design bug: overwrite of option (e.g. modules = ... \n modules = ...) is not caught (disabled due to pre-population on command line)
* combine moduleThreads and forceModuleThreads, maybe even modulePools to simplify

cmake
-----

* separate toolbox into its own repository
* add options to set -fno-exceptions and -fno-rtti (rtti needs some code changes)



POSTPONED
=========

* config: feature request -> meta modules [not yet]
* use std::regex(_match) and capture groups to check connector names and parse module lines (only alphanumeric) [C++11 suppport in gcc/clang?]
* control: move modules between master, thread and pool [GUI required]
* control: automagic module pooling [too much effort needed, requires deeper understanding]
* control: async mode [not needed/requested so far]
