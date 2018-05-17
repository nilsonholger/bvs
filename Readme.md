BVS - Broad Versatile Supervisor
================================



BASICS
------

	./run --help   -> list capabilities
	./run --setup  -> install some known modules
	./run --update -> update bvs and modules/...
	./run --clean  -> cleanup of ALL (c)make generated build files

**NOTE:**
`--update` runs `git stash; git pull; git stash pop` to keep local changes.
Please consider reading **Changelog.txt** after an update, as it might contain
some hints. Also, manual merging might be necessary.



BUILDING
--------

	cmake .      -> create makefiles
	make         -> build system
	make doc     -> build ALL documentation for framework and all modules
	make bvs-doc -> build documentation for framework ONLY

**RECOMMENDATION:**
Create a `build` directory and build there (`cd build; cmake ..`).

**NOTE:**
Point your browser to `[build/][lib/]doc/html/index.html` for the docs.



USAGE
-----

	./run --example                 -> run the framework example (requires OpenCV and camera)
	./run                           -> start using 'bvs.conf'
	./run $CONFIG_FILE              -> start system with user supplied config (relative to ./[build/]bin)
	./run --debug $CONFIG_FILE      -> start debug session
	./run --new-module $MODULE_NAME -> create new module
	./run --bvs.options=...         -> additional option presets (see BVS::Config docs)

**NOTE:**
When building the system using `cmake`, the system and all modules'
configuration files will be symlinked into the `[build]/bin` directory. You can
modify the original configuration files through these symlinks directly. You
won't have to worry about these changes being lost if you use the provided
'./run --update' function.



YOUR OWN MODULE
---------------

To create your own module:

	./run --new-module $MODULE_NAME   # creates module in 'modules/$MODULE_NAME'

Then start putting some functionality into your module. To connect to/from
other modules, you need to create `BVS::Connector` objects:

	# $MODULE_NAME.h
	BVS::Connector<int> input;
	BVS::Connector<std::string> output;
	
	# $MODULE_NAME.cc
	# initialization list:
	input("in", BVS::ConnectorType::INPUT),
	output("out", BVS::ConnectorType::OUTPUT),
	# execute():
	input.receive(...)
	output.send(...)

Furthermore, in your framework configuration, you need to specify the actual
connection between individual modules:

	modules = OtherModule   # defines output with name "out"
	
	# connects YourModule's input ("in") with OtherModule's "out"
	modules += YourModule.in(OtherModule.out)



LICENSE AND RESEARCH
--------------------

For licensing, please see `License.txt`. Also, if you happen to find this
software useful for your research, we would be happy if you cite the following
publication:

	@inproceedings{koesterICMEW2013accessible,
	  author    = {Daniel Koester and Boris Schauerte and Rainer Stiefelhagen},
	  title     = {Accessible Section Detection For Visual Guidance},
	  booktitle = {Multimedia and Expo Workshops (ICMEW), IEEE International Conference on},
	  year      = {2013},
	  month     = {July},
	  address   = {San Jose, CA},
	  doi={http://dx.doi.org/10.1109/ICMEW.2013.6618351}
	}



ADVANCED STUFF
--------------

### TREE STRUCTURE

	'.'
	 |_android:        android client
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



### DEVELOPMENT HINTS

The base repository can be detached from *origin* by renaming the remote, so
you can create your own base/upstream repository, or you can set its tracking
branch to it:

	git remote rename origin bvs-origin
	git branch master --set-upstream $YOUR_UPSTREAM

This allows you to
create your own remote *origin* for development whilst still being able to pull
updates to the base repository from the main BVS development repository.

### CONFIGS

It is strongly advisable to not use the generated config file links in
''build/bin'' when making local config changes, but to copy relevant configs
into your ''build/bin'' directory and modify those. It is also possible to put
all config sections (including the modules' ones) into a single config file.

### ANDROID CLIENT

The framework now has an android client. Further information about it's usage
can be found in `android/Readme.md`.

### BVS MODULE LIST

The file *.bvsmodules* contains a list of known modules. `./run --setup` asks
whether they should be (individually) installed. The format of each line in
this file MUST be as follows:

	${Module/CollectionName} ${PATH_TO_GIT_REPOSITORY} [COLLECTION]

e.g.: MyBVSModule git@my.git.server.tld:my-bvs-module.git

The first two parameters are required. `${PATH_TO_GIT_REPOSITORY}` can be
`ORIGIN:${SUFFIX}`, where `ORIGIN:` will be replaced by the same origin path
that was used for the main bvs repository. The parameter `COLLECTION` is
optional. It indicates that a repository contains more than one module. The
setup function will then create an entry in `modules/CMakeLists.txt` for every
directory it encounters in the collection's toplevel hierarchy.

### FIFO CONTROL

The BVS Daemon (bvsd) can be controlled through a FIFO file (`./run -f`). This
will (if necessary) create the special file `bin/bvsd-fifo` which can be used
to control bvsd by sending for example `echo "h" > $BIN_DIR/bvsd-fifo`. Using
`cat - > $BIN_DIR/bvsd-fifo` gives you a shell as before, but now input and
output are nicely separated. These are example maps to control bvsd from inside
vim:

	map <leader>ur :execute "! echo r > build/bin/bvsd-fifo"<cr><cr>
	map <leader>uh :execute "! echo hs " . expand('%:t:r') . " > build/bin/bvsd-fifo"<cr><cr>

