This is the BVS framework.



LICENSE:
to be decided...
by dk@hyve.org



INSTALLING:
To set everything up, type './run --setup'. This will download 'libbvs' and 'bvsd'
and set them up as git submodules. For more information, './run --help' will list
some of './run's capabilities.



BUILDING:
To generate Makefiles, type 'cmake .' or '$ENV_OPTS... cmake $CMAKE_OPTS... .'
After that, if you want to generate the doxygen documentation, type 'make doc'.
The documentation will be located in 'libbvs/doc/', so if you want to browse
the html documentation, point your browser to 'libbvs/doc/html/index.html' or
just read through the header/source files.
If you need/want to delete all cmake generated files and the documentation,
type './run --clean'.
For out of source builds, if you use the directory 'build', the run script will
still work.



USAGE:
A config example was copied to './bin/BVSConfig.txt'.
To start the system with the (default) config, type './run [config]' after
building the system (type 'make'). If you need to debug, type './run -d ...'
If you want to create a new module, type './run --new-module $MODULE_NAME'
and follow the instructions displayed by that script.



GIT REPOSITORY STRUCTURE:
This is the bvs base repository. Your checkout will, upon running
'./run --setup', rename its remote from 'origin' to 'bvs-origin'. This allows
you to create your own remote 'origin' for delevopment whilst still being able
to pull updates to the base repository from bvs' main development repository.
Furthermore you can create dedicated git repositories for each created module
and add them as git submodules to the base repository, or you can collect
your modules in a single git repository and either symlink them to
'./modules/...' or modify './modules/CMakeLists.txt' accordingly.




FILES AND DIRECTORIES:
'.'
 |- bin: directory where all your modules/executables/settings will be
 |       collected.
 |- bvsd: contains an interactive daemon (undocumented so far, just look
 |        at its source for more info)
 |- libbvs: contains the actual library, to get eventual updates, just
 |          update the git submodule
 |- modules: module directory, create git submodules here (do not forget
 |           to enable used modules in './modules/CMakeLists.txt')
 |- run
 |- CMakeLists.txt
 \- Readme.txt
