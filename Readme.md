BVS - Blind and Visullay impaired support System
================================================



INSTALLATION and UPDATES
------------------------

	./run --help   -> list capabilities
	./run --setup  -> install some known modules
	./run --update -> update bvs and modules/*
	./run --clean  -> cleanup of all generated build files (make and cmake)

**NOTE:**

`--update` actually runs `git stash`, `git pull` and `git stash pop`, so local
changes can be kept intact.  Please consider reading **Changelog.txt** after
doing an update, since it might contain some hints. Also, manual merging might
be necessary.



BUILDING
--------

	cmake .      -> create makefiles
	make         -> build system
	make doc     -> build ALL documentation for framework and all modules
	make bvs-doc -> build documentation for framework ONLY

**RECOMMENDED:**

Create a `build` directoy and build there (use `cmake ..`).

**NOTE:**

Point your browser to `[build/][lib/]doc/html/index.html` for the docs.



USAGE
-----

	./run --example                 -> run the framework example (requires OpenCV and camera)
	./run                           -> start using 'bvs.conf'
	./run --bvs.config=$CONFIG_FILE -> start system with user supplied config
	./run --debug $CONFIG_FILE      -> start debug session
	./run --new-module $MODULE_NAME -> create new module
	./run --bvs.options=...         -> additional option presets (see BVS::Config docs)

**NOTE:**

When building the system using `cmake`, the system and all modules' config
files will be symlinked into the `[build]/bin` directory. You can modify the
original config files through these symlinks directly. You won't have to worry
about these changes being lost if you use the provided update function.



CREATE AND USE YOUR OWN MODULE
------------------------------

To create your own module, please use `./run --new-module $MODULE_NAME`. Then
start putting some functionality into your module located in
`modules/$MODULE_NAME`. To connect your module with other modules, you need to
create `BVS::Connector`'s. This could be done for example by declaring
Connectors in your module's header:

	BVS::Connector<int> input;
	BVS::Connector<std::string> output;

as well as its corresponding declaration in the constructor's initialization
list:

	...
	input("in", BVS::ConnectorType::INPUT),
	output("out", BVS::ConnectorType::OUTPUT),
	...

Then, in the `execute()` function, use `input.receive(...)` and
`output.send(...)` to recieve and send your content from/to other modules.

Furthermore, in your framework configuration, you need to specify the actual
connection between individual modules:

	modules = OtherModule   # defines output with name "out"
	modules += YourModule.in(OtherModule.out)
	# above connects YourModule's input named "in" with OtherModule's "out"



ADVANCED STUFF
--------------

### TREE STRUCTURE:

	'.'
	 |_[build]:        OPTIONAL directory for out of source builds
	 |_[build/]bin:    directory where all your modules/executables/settings will be
	 |_daemon:         contains an interactive command line daemon
	 |_lib:            contains the actual framework library
	 |_modules:        module directory, create git submodules here (do not forget
	 |                 to enable used modules in './modules/CMakeLists.txt')
	 |_.bvsmodules:    list of known modules (used during setup)
	 |_run:            main script to do various tasks
	 |_Changelog.txt:  contains important notes about updates
	 |_CMakeLists.txt: cmake build file
	 |_Doxyfile.in:    doxygen config, parsed by cmake
	 |_Licensce.txt:   legalese stuff
	 |_Readme.txt:     this readme file



### DEVELOPMENT HINTS:

The base repository can be detached from *origin* by renaming the remote, e.g.
`git remote rename origin bvs-origin`, so you can create your own base
repository with its own upstream, or you can set its tracking branch to it,
e.g. `git branch master --set-upstream $YOUR_UPSTREAM`.  This allows you to
create your own remote *origin* for delevopment whilst still being able to pull
updates to the base repository from bvs' main development repository.



### BVS MODULE LIST:

The file *.bvsmodules* contains a list of known modules. `./run --setup` asks
whether they should be (individually) installed.  The format of each line in
this file MUST be as follows:

	${Module/CollectionName} ${PATH_TO_GIT_REPOSITORY} [COLLECTION]

e.g.: MyBVSModule git@my.git.server.tld:my-bvs-module.git

The first two parameters are required. `${PATH_TO_GIT_REPOSITORY}` can be
`ORIGIN:${SUFFIX}`, where `ORIGIN:` will be replaced by the same origin path
that was used for the main bvs repository.  The parameter `COLLECTION` is
optional.  It indicates that a repository contains more than one module. The
setup function will then create an entry in `modules/CMakeLists.txt` for
every directory it encounters in the collection's toplevel hierarchy.



### FIFO CONTROL:

The BVS Daemon (bvsd) can be controlled through a FIFO file (`./run -f`). This
will (if necessary) create the special file `bin/bvsd-fifo` which can be used
to control bvsd by sending for example `echo "h" > $BIN_DIR/bvsd-fifo`.  Using
`cat - > $BIN_DIR/bvsd-fifo` gives you a shell as before, but now input and
output are nicely separated.  These are example maps to control bvsd from
inside vim:

	map <leader>ur :execute "! echo r > build/bin/bvsd-fifo"<cr><cr>
	map <leader>uh :execute "! echo hs " . expand('%:t:r') . " > build/bin/bvsd-fifo"<cr><cr>

