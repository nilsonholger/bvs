#ifndef BVS_CONTROLDATA_H
#define BVS_CONTROLDATA_H

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
	/** Possible control flags (self explanatory). */
	enum class ControlFlag { QUIT = 0, WAIT = 1, RUN = 2 };



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
		 * @param[in] poolName The pool name executing this module (if any).
		 * @param[in] flag System control flag for module.
		 * @param[in] status Return Status of module functions.
		 * @param[in] connectors Connector vector.
		 */
		ModuleData(std::string id, std::string library, std::string options,
				Module* module, void* dlib, bool asThread, std::string poolName,
				ControlFlag flag, Status status, ConnectorMap connectors)
			: id(id),
			library(library),
			options(options),
			module(module),
			dlib(dlib),
			thread(),
			asThread(asThread),
			poolName(poolName),
			flag(flag),
			status(status),
			connectors(connectors)
		{}

		std::string id; /**< Name of module. */
		std::string library; /**< Library to load module from. */
		std::string options; /**< Module options. */
		std::shared_ptr<Module> module; /**< Pointer to the module. */
		void* dlib; /**< Dlib handle to module's lib. */
		std::thread thread; /**< Thread handle of module. */
		bool asThread; /**< Determines if module runs in its own thread. */
		std::string poolName; /** The pool name executing this module (if any). */
		ControlFlag flag; /**< System control flag for module. */
		Status status; /**< Return Status of module functions. */
		ConnectorMap connectors; /**< Connector vector. */

		ModuleData(const ModuleData&) = delete; /**< -Weffc++ */
		ModuleData& operator=(const ModuleData&) = delete; /**< -Weffc++ */
	};



	/** Module Map. */
	typedef std::map<std::string, std::shared_ptr<ModuleData>, std::less<std::string>> ModuleMap;

	/** Module Vector.*/
	typedef std::vector<std::shared_ptr<ModuleData>> ModuleVector;




	/** Pool metadata. */
	struct PoolData
	{
		/** Creates pool metadata.
		 * @param[in] poolName Pool name.
		 * @param[in] flag Control flag.
		 */
		PoolData(std::string poolName, ControlFlag flag)
			: poolName(poolName),
			flag(flag),
			thread(),
			modules()
		{}

		std::string poolName; /**< Pool name. */
		ControlFlag flag; /**< System control flag for pool. */
		std::thread thread; /**< Pool thread handle. */
		ModuleVector modules; /**< Pool module vector. */
	};



	/** Module Pool Map. */
	typedef std::map<std::string, std::shared_ptr<PoolData>> PoolMap;



} // namespace BVS



#endif //BVS_CONTROLDATA_H

