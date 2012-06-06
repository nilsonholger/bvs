#ifndef BVSMODULE_H
#define BVSMODULE_H

#include "BVS.h"



/** Base class for BVS Modules.
 * This is the base class for BVS Modules.
 * Extend this class and implement the functions below.
 */
class BVSModule
{
    // TODO COMMENTS to explain usage and give examples
    public:
        BVSModule(BVSConfig& config); /**< Constructor of Module Base. */
        virtual ~BVSModule(); /**< Destructor of Module Base. */
        virtual BVSStatus onLoad() = 0;
        virtual BVSStatus preExecute() = 0;
        virtual BVSStatus execute() = 0;
        virtual BVSStatus postExecute() = 0;
        virtual BVSStatus debugDisplay() = 0;
        virtual BVSStatus onClose() = 0;

    protected:
        // TODO finish moving from module to base class
        BVSConfig& config;

    private:
        BVSModule(const BVSModule&) = delete; /**< -Weffc++ */
        BVSModule& operator=(const BVSModule&) = delete; /**< -Weffc++ */

};

#endif //BVSMODULE_H

