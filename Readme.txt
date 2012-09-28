This is the BVS framework.



LICENSE:
to be decided...until then, copyright by dk@hyve.org



INSTALLING:
To set everything up, type './run --setup'. This will download 'libbvs' and
'bvsd' and set them up as git submodules. For more information, './run --help'
will list './run's capabilities.



UPDATING:
To update submodules, type './run --update'. This will run 'git stash',
'git pull' and 'git stash pop' in the base and submodule directories.
To ensure it is not messing with your own stashes, it will record where it had
to do a 'git stash' and to a 'git stash pop' in those recorded locations only.



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
A config example can be found in './libbvs/BVSConfig.txt.sample'. You will need
to copy it to your 'bin' directory (either in the source tree or in an out of
source build, e.g. 'build').

To start the system with a config file (default=BVSConfig.txt), type './run' or
'./run $config' after building the system (type 'make').

If you need to debug, type './run --debug ...'.

If you want to create a new module, type './run --new-module $MODULE_NAME' and
follow the instructions displayed by it.



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
