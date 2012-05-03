#ifndef OUTPUT_H
#define OUTPUT_H

#include "BVSModule.h"

class OUTPUT : public BVSModule
{
    public:
        OUTPUT(BVSConfig& config);
        ~OUTPUT();
        BVSStatus onLoad();
        BVSStatus preExecute();
        BVSStatus execute();
        BVSStatus postExecute();
        BVSStatus debugDisplay();
        BVSStatus onClose();

    private:
        std::string identifier;
        BVSLogger logger;
        BVSConfig& config;
        OUTPUT(const OUTPUT&) = delete; /**< -Weffc++ */
        OUTPUT& operator=(const OUTPUT&) = delete; /**< -Weffc++ */
};

#endif //OUTPUT_H

