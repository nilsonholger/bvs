#include "stdio.h"
#include "BVS.h"

int main(int argc, char** argv)
{
    // start BVS
    BVS bvs(argc, argv);
    bvs.setLogSystemVerbosity(3);
    bvs.enableLogFile("BVSLog.txt");

    //bvs.setup(); // load and apply options from config

    BVSLogger logger("BVSDaemon");
    LOG(2, "starting!");

    bvs.loadModules();


    // TODO either cleanup or turn into conditional test
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


    int foo;
    std::string bar;
    bool test;
    bvs.config.getValue("core.foo", foo)
        .getValue("core.bar", bar)
        .getValue("core.test", test);

    LOG(0, "Getting foo: " << foo);
    LOG(0, "Getting bar: " << bar);
    LOG(0, "BOOL: " << test);

    foo = bvs.config.getValue<int>("core.foo");
    bar = bvs.config.getValue<std::string>("core.bar");
    test = bvs.config.getValue<bool>("core.test");
    LOG(0, "Getting foo directly: " << foo);
    LOG(0, "Getting bar directly: " << bar);
    LOG(0, "BOOL directly: " << test);

    std::string bar2;
    std::string bar3;
    bvs.config.getValue("core.bar", bar2);
    bar3 = bvs.config.getValue<std::string>("core.bar");
    LOG(0, "bar2: " << bar2);
    LOG(0, "bar3: " << bar3);

    bool test2;
    bool test3;
    bvs.config.getValue("core.test", test2);
    test3 = bvs.config.getValue<bool>("core.test");
    LOG(0, "test2: " << test2);
    LOG(0, "test3: " << test3);

    std::vector<std::string> list;
    bvs.config.getValue("core.list", list);
    for (auto it : list)
    {
        LOG(0, it);
    }

    return 0;
}
