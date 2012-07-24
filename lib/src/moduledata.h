#ifndef BVS_MODULEDATA_H
#define BVS_MODULEDATA_H

#include<atomic>
#include<map>
#include<memory>
#include<string>
#include<thread>
#include<vector>

#include "bvs/connector.h"
#include "bvs/module.h"



/** BVS namespace, contains all library stuff. */
namespace BVS
{
	/** Possible module flags (self explanatory). */
	enum class ModuleFlag { QUIT = 0, WAIT = 1, RUN = 2 };



	/** Module metadata. */
	struct ModuleData
	{
		std::string id; /**< Name of module. */
		std::string library; /**< Library to load module from. */
		std::string options; /**< Module options. */
		Module* module; /**< Pointer to the module. */
		void* dlib; /**< Dlib handle to module's lib. */
		std::thread thread; /**< Thread handle of module. */
		bool asThread; /**< Determines if module runs in its own thread. */
		ModuleFlag flag; /**< System control flag for module. */
		Status status; /**< Return Status of module functions. */
		ConnectorMap connectors; /**< Connector vector. */
	};
} // namespace BVS



#endif //BVS_MODULEDATA_H

