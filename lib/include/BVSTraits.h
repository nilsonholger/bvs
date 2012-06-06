#ifndef BVSTRAITS_H
#define BVSTRAITS_H

#include<map>
#include<memory>
#include<string>
#include<thread>



/** Disables the logging system. */
//#define BVS_LOG_DISABLED



/** BVSStatus possibilities. */
enum BVSStatus { NONE = 0, OK = 1, WAIT = 2, FAIL = 4, NOINPUT = 8 };

/** Module ID. */
typedef unsigned int BVSModuleID;

// Forward Declaration
class BVSModule;

/** Module metadata. */
struct BVSModuleData
{
	BVSModuleID id; /**< ID of module. */
	std::string name; /**< Name of module. */
	BVSModule* module; /**< Pointer to the module. */
	void* dlib; /**< Dlib handle to module's lib. */
	bool asThread; /**< Determines if module runs in its own thread. */
	std::thread thread; /**< Thread handle of module. */
};

/** Module List. */
typedef std::map<BVSModuleID, std::shared_ptr<BVSModuleData>, std::less<unsigned int>> BVSModuleMap;



#endif //BVSTRAITS_H

