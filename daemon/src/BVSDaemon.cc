#include "stdio.h"
#include "BVS.h"

int main(int argc, char** argv)
{
    // start BVS
    BVS bvs(argc, argv);
    bvs.setLogSystemVerbosity(3);
    bvs.enableLogFile("BVSLog.txt");

    //bvs.setup(); // load and apply options from config

    //BVSConfig config("BVSDaemon");

    BVSLogger logger("BVSDaemon");
    LOG(2, "Starting!");

    //bvs.loadModules();


    /*
    LOG(0, "CLI");
    LOG(0, "CLI FILE");
    bvs.disableLogConsole();
    LOG(0, "FILE");
    BVSLogger file("FILE", 3, BVSLogger::TO_FILE);
    file.out(0) << "FILE ONLY" << std::endl;
    bvs.enableLogConsole();
    BVSLogger cli("CLI", 3, BVSLogger::TO_CLI);
    cli.out(0) << "CLI ONLY" << std::endl;
    bvs.disableLogConsole();
    bvs.disableLogFile();
    LOG(0, "NOOP");
    bvs.enableLogConsole();
    LOG(0, "CLI");
    bvs.disableLogConsole();
    bvs.enableLogFile("BVSLog.txt", true);
    LOG(0, "FILE AGAIN");
    bvs.enableLogConsole();
    BVSLogger both("BOTH", 3, BVSLogger::TO_CLI_AND_FILE);
    both.out(0) << "CLI AND FILE" << std::endl;
    */


    /*
    int foo;
    std::string bar;
    config.getValue("f,foo", foo)
        .getValue("bar,b", bar);
    LOG(0, "Getting foo: " << foo);
    LOG(0, "Getting bar: " << bar);
    int foo2 = config.getValue<int>("f,foo");
    LOG(0, "Getting long: " << foo2);
    config.getValue("long", foo2);
    LOG(0, "Getting long: " << foo2);
    config.getValue("s", foo2);
    LOG(0, "Getting s: " << foo2);
    LOG(0, "BOOL: " << true);
    */

    return 0;
}
