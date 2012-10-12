This is the BVS framework.



LICENSE:
to be decided...until then, copyright by dk@hyve.org



INSTALLING:
To set everything up, type './run --setup'. This will download the bvs library
and its clients as well as set them up as git submodules. Furthermore you will
be asked whether you want to install a list of known modules, which will also
be added as git submodules.

For more information, './run --help' will list './run's capabilities.

WARNING: setup creates a commit (if new submodules were created), which must
NEVER be pushed to a development repository, UNLESS you want to share your
setup with others. This might break things, you have been warned.



UPDATING:
To update submodules, type './run --update'. This will run 'git stash',
'git pull' and 'git stash pop' in the base and submodule directories.
To ensure it is not messing with your own stashes, it will record where it had
to do a 'git stash' and to a 'git stash pop' in those recorded locations only.

Please consider reading Changelog.txt after doing an update, since it might
contain hints abount changed things which could cause problems. Even if you
do not read it after every update, look there first if something breaks.

WARNING: if you do a direct pull in a submodule, be sure you know what you are
doing, especially since you might loose your own config settings etc.



BUILDING:
To generate Makefiles, type 'cmake .' or '$ENV_OPTS... cmake $CMAKE_OPTS... .'
for in source builds, or 'cmake $PATH_TO_BVS' for out of source builds (if you
use the directory 'build', the run script will still work).

After that, if you want to generate the doxygen documentation, type 'make doc'.
The documentation will be located in 'libbvs/doc/', so if you want to browse
the html documentation, point your browser to 'libbvs/doc/html/index.html' or
just read through the header/source files.

If you need/want to delete all cmake generated files and the documentation,
type './run --clean'.



USAGE:
Upon building the system using 'cmake', the system and all modules' config
files will be symlinked into the '[build]/bin' directory. You can modify the
original config files through these symlinks directly. You won't have to worry
about these changes being lost if you use the provided update function (see
UPDATING).

To start the system with a config file (default=BVSConfig.txt), type './run' or
'./run $config' after building the system (type 'make').

If you need to debug, type './run --debug ...'.

If you want to create a new module, type './run --new-module $MODULE_NAME' and
follow the instructions displayed by it.

If you want to use a different config file, type './run --bvs.config=$CONFIG'
and you can also append '--bvs.options=...' to override options (see the Config
documentation for further details).



BVS MODULE LIST:
The file '.bvsmodules' contains a list of known modules. Upon setup, you will
be asked whether you want to (individually) install those.

The format of this file is as follows:

	ModuleName PATH_TO_GIT_REPOSITORY
	RepositoryName PATH_TO_GIT_REPOSITORY Module1 Module2 [...]

These two formats are the ONLY allowed ones, nothing else, no empty lines, no
comments.



INTENDED TREE STRUCTURE:
This is the bvs base repository. It's purpose is to serve as a common base for
the BVS framework, its clients as well as its modules.

You can detach the base repository from 'origin' by renaming the remote, e.g.
'git remote rename origin bvs-origin', so you can create your own base
repository with its own upstream, or you can set its tracking branch to it,
e.g. 'git branch master --set-upstream $YOUR_UPSTREAM'.
This allows you to create your own remote 'origin' for delevopment whilst still
being able to pull updates to the base repository from bvs' main development
repository.

Furthermore, you can create dedicated git repositories for each module and add
them as git submodules to the base repository, or you can collect your modules
in a single git repository and either symlink them to './modules/...' or modify
'./modules/CMakeLists.txt' accordingly.




DIRECTORIES AND FILES:
'.'
 |- bin: directory where all your modules/executables/settings are
 |- build: OPTIONAL directory for out of source builds
 |- bvsd: contains an interactive command line daemon
 |- libbvs: contains the actual library,
 |- modules: module directory, create git submodules here (do not forget
 |           to enable used modules in './modules/CMakeLists.txt')
 |
 |- run: main script to do various tasks
 |- Changelog.txt: contains important notes about updates
 |- CMakeLists.txt: cmake build file
 |- Doxyfile.in: doxygen config, parsed by cmake
 \- Readme.txt: this readme file
