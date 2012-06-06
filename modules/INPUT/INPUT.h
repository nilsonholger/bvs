#ifndef INPUT_H
#define INPUT_H

#include "BVSModule.h"

class INPUT : public BVSModule
{
    public:
        INPUT(BVSConfig& config);
        ~INPUT();
        BVSStatus onLoad();
        BVSStatus preExecute();
        BVSStatus execute();
        BVSStatus postExecute();
        BVSStatus debugDisplay();
        BVSStatus onClose();

    private:
        std::string identifier;
        BVSLogger logger;
		// TODO remove from here
        //BVSConfig& config;
        INPUT(const INPUT&) = delete; /**< -Weffc++ */
        INPUT& operator=(const INPUT&) = delete; /**< -Weffc++ */
};

#endif //INPUT_H

