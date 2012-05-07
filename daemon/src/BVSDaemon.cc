#include "stdio.h"
#include "BVS.h"

int main(int argc, char** argv)
{
    // start BVS
    BVS bvs(argc, argv);
    //bvs.enableLogFile("BVSLog.txt");
    //bvs.enableLogConsole();

    BVSLogger logger("BVSDaemon");
    LOG(2, "starting!");

    //LOG(2, "dump all config options!");
    //bvs.config.showOptionStore();

    LOG(2, "loading Modules!");
    bvs.loadModules();

    LOG(2, "run!");
    bvs.run();

    bvs.close();

    return 0;
}
