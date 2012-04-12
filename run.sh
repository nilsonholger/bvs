#! /bin/sh
# dk@hyve.org

function bvs_clean {
echo ">>> make clean"
make clean
DIRS="./"
[ -r bvsd ] && DIRS+=" bvsd/"
[ -r libbvs ] && DIRS+=" libbvs/"
[ -r modules ] && DIRS+=" modules/"
for i in `ls -d modules/*/`
do
    [ $i != "modules/CMakeFiles/" ] && DIRS+=" $i"
done

#DIRS=${DIRS/ modules\/BVSExampleModule\/}

echo ">>> cmake cleanup: $DIRS"
rm CMakeCache.txt
for i in $DIRS
do
    rm $i/Makefile
    rm $i/cmake_install.cmake
    rm -r $i/CMakeFiles
done
}

function bvs_help {
cat << EOF
usage: $0 [command [arg]]
  clean         run make clean and remove all CMake generated content
  debug         runs system in gdb session
  help          displays this text
  new-module    creates new module blank with given name
  run (default) runs the system
  setup         this adds libbvs and bvsd
EOF
}

function bvs_new_module {
[ -z $1 ] && echo "No module name given!" && exit 1
ORIG=BVSExampleModule
ORIGC=`echo $ORIG | tr "[:lower:]" "[:upper:]"`
MOD=$1
MODC=`echo $MOD | tr "[:lower:]" "[:upper:]"`
cp -r modules/$ORIG modules/$MOD
cd modules/$MOD
mv $ORIG.h $MOD.h
mv $ORIG.cc $MOD.cc
FILES="CMakeLists.txt $MOD.h $MOD.cc"
sed -i '' -e "s/$ORIG/$MOD/g" $FILES
sed -i '' -e "s/$ORIGC/$MODC/g" $FILES
cd ../..
echo "#add_subdirectory($MOD)" >> modules/CMakeLists.txt
echo "Created $MOD, uncomment add_subdirectory($MOD) in modules/CMakeLists.txt to add it to compilation!"
}

function bvs_setup {
mkdir bin
git remote rm origin
BASE="ssh://git.hyve.org:119"
git submodule add $BASE/libbvs.git
git submodule add $BASE/bvsd.git
cmake .
}

function bvs_run {
[ ! -r bin/bvsd ] && echo "bvsd executable not found, run make first..." && exit
(cd bin && $GDB ./bvsd --bvs.config=BVSConfig.txt --bvs.options=foo=123,bar=foobar)
}


case $1 in
    clean) bvs_clean;;
    debug) GDB="gdb --args "; bvs_run;;
    help) bvs_help;;
    new-module) bvs_new_module $2;;
    setup) bvs_setup;;
    "") bvs_run;;
esac
