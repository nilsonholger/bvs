#ifndef INPUT_H
#define INPUT_H

#include "BVSModule.h"

class INPUT : public BVSModule
{
    public:
        INPUT(BVSConfig& config);
        ~INPUT();
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
        INPUT(const INPUT&) = delete; /**< -Weffc++ */
        INPUT& operator=(const INPUT&) = delete; /**< -Weffc++ */
};

#endif //INPUT_H

