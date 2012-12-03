This is the BVS framework.



LICENSE:
Please see 'License.txt'



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
The documentation will be located in 'lib/doc/', so if you want to browse the
html documentation, point your browser to '[build/]lib/doc/html/index.html' or
just read through the header/source files.

If you need/want to delete all cmake generated files and the documentation,
type './run --clean'.



USAGE:
Upon building the system using 'cmake', the system and all modules' config
files will be symlinked into the '[build]/bin' directory. You can modify the
original config files through these symlinks directly. You won't have to worry
about these changes being lost if you use the provided update function (see
UPDATING).

To start the system with a config file (default=bvs.conf), type './run' or
'./run $config' after building the system (type 'make').

If you need to debug, type './run --debug ...'.

If you want to create a new module, type './run --new-module $MODULE_NAME' and
follow the instructions displayed by it.

If you want to use a different config file, type './run --bvs.config=$CONFIG'
and you can also append '--bvs.options=...' to override options (see the Config
documentation for further details).

If you want to control bvsd through a FIFO file, type './run -f'. This will (if
necessary) create the special file 'bvsd-fifo' which you can use to control
bvsd by sending for example: 'echo "h" > $BIN_DIR/bvsd-fifo'.
Using 'cat - > $BIN_DIR/bvsd-fifo' gives you a shell as before, but now input
and output ar nicely separated.

To control bvsd from inside vim, you could set the following maps inside your
vimrc (these assume that you opened your vim session inside the bvs base
directory and use 'build' for an out of source build, also they are optimized
for a dvorak keyboard layout ;-):

	" bvsd-fifo control
	map <leader>ui :execute "! echo r > build/bin/bvsd-fifo"<cr><cr>
	map <leader>uu :execute "! echo s > build/bin/bvsd-fifo"<cr><cr>
	map <leader>ud :execute "! echo p > build/bin/bvsd-fifo"<cr><cr>
	map <leader>ue :execute "! echo q > build/bin/bvsd-fifo"<cr><cr>
	map <leader>uh :execute "! echo hs " . expand('%:t:r') . " > build/bin/bvsd-fifo"<cr><cr>



BVS MODULE LIST:
The file '.bvsmodules' contains a list of known modules. Upon setup, you will
be asked whether you want to (individually) install those.

The format of EACH line in this file must be as follows:

	${Module/CollectionName} ${PATH_TO_GIT_REPOSITORY} [COLLECTION]

e.g.: MyBVSModule git@my.git.server.tld:mybvsmodule.git

The first two parameters are required. The parameter 'COLLECTION' is optional.
It indicates that a repository contains more than one module. The 'setup'
function will then create an entry in 'modules/CMakeLists.txt' for every
directory it encounters in the collection's toplevel hierarchy.



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
 |_bin: directory where all your modules/executables/settings are
 |_build: OPTIONAL directory for out of source builds
 |_bvsd: contains an interactive command line daemon
 |_lib: contains the actual library
 |_modules: module directory, create git submodules here (do not forget
 |           to enable used modules in './modules/CMakeLists.txt')
 |
 |_.bvsmodules: list of known modules (used during setup)
 |_run: main script to do various tasks
 |_Changelog.txt: contains important notes about updates
 |_CMakeLists.txt: cmake build file
 |_Doxyfile.in: doxygen config, parsed by cmake
 |_Licensce.txt: legalese stuff
 |_Readme.txt: this readme file
