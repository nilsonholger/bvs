#ifndef BVSMODULE_H
#define BVSMODULE_H

#include "BVS.h"



/** Base class for BVS Modules.
 * This is the base class for BVS Modules.
 * Extend this class and implement the functions below.
 */
class BVSModule
{
    public:
        BVSModule() = default; /**< Constructor of Module Base. */
        virtual ~BVSModule(); /**< Destructor of Module Base. */
        virtual void onLoad() = 0;
        virtual void preExecute() = 0;
        virtual void execute() = 0;
        virtual void postExecute() = 0;
        virtual void debugDisplay() = 0;
        virtual void onClose() = 0;

    private:
        BVSModule(const BVSModule&) = delete; /**< -Weffc++ */
        BVSModule& operator=(const BVSModule&) = delete; /**< -Weffc++ */

};

extern std::map<std::string, BVSModule*, std::less<std::string>> bvsModules;

#endif //BVSMODULE_H

