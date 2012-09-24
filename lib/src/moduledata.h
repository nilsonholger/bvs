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
		/** Creates Module MetaData.
		 * @param[in] id Name of module.
		 * @param[in] library Library to load module from.
		 * @param[in] options Module options.
		 * @param[in] module Pointer to the module.
		 * @param[in] dlib Dlib handle to module's lib.
		 * @param[in] asThread Determines if module runs in its own thread.
		 * @param[in] flag System control flag for module.
		 * @param[in] status Return Status of module functions.
		 * @param[in] connectors Connector vector.
		 */
		ModuleData(std::string id, std::string library, std::string options,
				Module* module, void* dlib, bool asThread, ModuleFlag flag,
				Status status, ConnectorMap connectors)
			: id(id)
			, library(library)
			, options(options)
			, module(module)
			, dlib(dlib)
			, thread()
			, asThread(asThread)
			, flag(flag)
			, status(status)
			, connectors(connectors)
		{}

		std::string id; /**< Name of module. */
		std::string library; /**< Library to load module from. */
		std::string options; /**< Module options. */
		Module* module; /**< Pointer to the module. */
		void* dlib; /**< Dlib handle to module's lib. */
		std::thread thread; /**< Thread handle of module. */
		bool asThread; /**< Determines if module runs in its own thread. */
		std::atomic<ModuleFlag> flag; /**< System control flag for module. */
		Status status; /**< Return Status of module functions. */
		ConnectorMap connectors; /**< Connector vector. */

		ModuleData(const ModuleData&) = delete; /**< -Weffc++ */
		ModuleData& operator=(const ModuleData&) = delete; /**< -Weffc++ */
	};
} // namespace BVS



#endif //BVS_MODULEDATA_H

