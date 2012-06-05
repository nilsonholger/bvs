#ifndef BVSMODULE_H
#define BVSMODULE_H

#include "BVS.h"



/** BVSStatus possibilities.
 *
 */
enum BVSStatus { NONE = 0, OK = 1, WAIT = 2, FAIL = 4, NOINPUT = 8 };



/** Base class for BVS Modules.
 * This is the base class for BVS Modules.
 * Extend this class and implement the functions below.
 */
class BVSModule
{
    // TODO COMMENTS to explain usage and give examples
    public:
        BVSModule() = default; /**< Constructor of Module Base. */
        virtual ~BVSModule(); /**< Destructor of Module Base. */
        virtual BVSStatus onLoad() = 0;
        virtual BVSStatus preExecute() = 0;
        virtual BVSStatus execute() = 0;
        virtual BVSStatus postExecute() = 0;
        virtual BVSStatus debugDisplay() = 0;
        virtual BVSStatus onClose() = 0;

    private:
        BVSModule(const BVSModule&) = delete; /**< -Weffc++ */
        BVSModule& operator=(const BVSModule&) = delete; /**< -Weffc++ */

};

#endif //BVSMODULE_H

