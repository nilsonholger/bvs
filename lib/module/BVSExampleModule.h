#ifndef BVSEXAMPLEMODULE_H
#define BVSEXAMPLEMODULE_H

#include "BVSModule.h"

class BVSExampleModule : public BVSModule
{
    public:
        BVSExampleModule(BVSConfig& config);
        ~BVSExampleModule();
        void onLoad();
        void preExecute();
        void execute();
        void postExecute();
        void debugDisplay();
        void onClose();

    private:
        std::string identifier;
        BVSLogger logger;
        BVSConfig& config;
        BVSExampleModule(const BVSExampleModule&) = delete; /**< -Weffc++ */
        BVSExampleModule& operator=(const BVSExampleModule&) = delete; /**< -Weffc++ */
};

#endif //BVSEXAMPLEMODULE_H

