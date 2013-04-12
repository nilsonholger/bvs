BVS - Blind and Visullay impaired support System
================================================



INSTALLATION and UPDATES
========================

'./run --help'   -> will list './run's capabilities
'./run --setup'  -> asks to download some known modules
'./run --update' -> updates bvs and all its (git sub-)modules
'./run --clean'  -> cleanup of ALL generated build files

WARNING: '--setup' creates a commit (if new submodules were created), which
MUST NEVER be pushed to a development repository, UNLESS you want to share your
setup with others. This process might break things, you have been warned.

NOTE: '--update' will actually run 'git stash', 'git pull' and 'git stash pop',
so local changes can be kept intact.  Please consider reading Changelog.txt
after doing an update, since it might contain some hints.



BUILDING
========

'cmake .' -> creates makefiles
'make' -> builds system
'make doc' -> builds ALL documentation for framework and all modules
'make bvs-doc' -> builds documentation for framework ONLY

RECOMMENDED: create a 'build' directoy and build there (use 'cmake ..').

NOTE: Point your browser to '[build/][lib/]doc/html/index.html' for the docs.



USAGE
=====

'./run --example'                 -> run the framework example (requires OpenCV and camera)
'./run'                           -> starts using 'bvs.conf'
'./run --bvs.config=$CONFIG_FILE' -> starts system with user supplied config
'./run --debug $CONFIG_FILE'      -> starts debug session
'./run --new-module $MODULE_NAME' -> creates new module
'./run --bvs.options=...'         -> additional option presets (see Config docs)

NOTE: When building the system using 'cmake', the system and all modules'
config files will be symlinked into the '[build]/bin' directory. You can modify
the original config files through these symlinks directly. You won't have to
worry about these changes being lost if you use the provided update function.



DIRECTORIES AND FILES
=====================

'.'
 |_[build]: OPTIONAL directory for out of source builds
 |_[build/]bin: directory where all your modules/executables/settings will be
 |_daemon: contains an interactive command line daemon
 |_lib: contains the actual framework library
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



ADVANCED STUFF
==============

INTENDED TREE STRUCTURE:
You can detach the base repository from 'origin' by renaming the remote, e.g.
'git remote rename origin bvs-origin', so you can create your own base
repository with its own upstream, or you can set its tracking branch to it,
e.g. 'git branch master --set-upstream $YOUR_UPSTREAM'.
This allows you to create your own remote 'origin' for delevopment whilst still
being able to pull updates to the base repository from bvs' main development
repository.



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



FIFO CONTROL:
If you want to control bvsd through a FIFO file, type './run -f'. This will (if
necessary) create the special file 'bvsd-fifo' which you can use to control
bvsd by sending for example: 'echo "h" > $BIN_DIR/bvsd-fifo'.
Using 'cat - > $BIN_DIR/bvsd-fifo' gives you a shell as before, but now input
and output are nicely separated.
To control bvsd from inside vim, you could add some maps to your vimrc:

	map <leader>ur :execute "! echo r > build/bin/bvsd-fifo"<cr><cr>
	map <leader>uh :execute "! echo hs " . expand('%:t:r') . " > build/bin/bvsd-fifo"<cr><cr>


