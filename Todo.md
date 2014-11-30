TODO
====

bvs
---
* GUI!!!
* control: move modules between master, thread and pool [GUI required]
* connectors: allow simultaneous reading of input if multiple modules connect to the same output
* binary data dump: record/replay generic data dumps (no need to write a recorder/reader for all kinds of input data)
* hotswap: with external struct
* logger: level 0 (ERROR) requests shutdown
* logger: in memory logging -> lock free
* control: add mapping of thread/pool to cpu (pthread_setaffinity_np)

code
----
* compact ALL objects (objects should be mostly DATA), create *mostly* pure functions, make them friend functions if they need to modify private data

testing
-------
* add testing directory 'lib/t/' with scripts and .cc files
* create (regression) test suite using cmake CTEST
* improve code quality with gcov and valgrind

config
------
* design bug: overwrite of option (e.g. modules = ... \n modules = ...) is not caught (disabled due to pre-population on command line)

POSTPONED
=========
* cmake: add options to set -fno-exceptions and -fno-rtti (rtti needs some code changes)
* cmake: separate toolbox into its own repository
* config: feature request -> meta modules [not yet]
* use std::regex_match() and capture groups to check connector names and parse module lines (only alphanumeric) [C++11 suppport in gcc/clang?]
* control: automagic module pooling [too much effort needed, requires deeper dependency understanding]
* control: async mode [not needed/requested so far]
