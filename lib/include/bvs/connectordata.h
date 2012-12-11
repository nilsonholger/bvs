#ifndef BVS_CONNECTORDATA_H
#define BV__CONNECTORDATA_H

#include <mutex>
#include <iostream>
#include <map>
#include <memory>
#include <string>

#include "bvs/traits.h"



/** BVS namespace, contains all library stuff. */
namespace BVS
{
	/** Connector Types.
	 * NOOP   - not assigned
	 * INPUT  - used for input
	 * OUTPUT - used for output
	 */
	enum class ConnectorType { NOOP, INPUT, OUTPUT};



	/** Ostream operator for ConnectorType. */
	inline std::ostream& operator<< (std::ostream& os, const ConnectorType type)
	{
		switch (type)
		{
			case ConnectorType::INPUT:
				os << "INPUT";
				break;
			case ConnectorType::OUTPUT:
				os << "OUTPUT";
				break;
			default:
				os << "UNDEFINED";
		}
		return os;
	}



	/** Connector meta data store. */
	struct ConnectorData
	{
		/** Creates Connector MetaData.
		 * @param[in] id Identifier.
		 * @param[in] type Type.
		 * @param[in] active If connector is active/assigned.
		 * @param[in] pointer Void pointer to contained object.
		 * @param[in] typeIDHash Hash code of templated type.
		 * @param[in] typeIDName Type of template instantiation.
		 * @param[in] locked If connection is locked.
		 */
		ConnectorData(std::string id, ConnectorType type, bool active, std::shared_ptr<void> pointer,
				size_t typeIDHash, std::string typeIDName, bool locked)
			: id{id},
			type{type},
			active{active},
			pointer{pointer},
			typeIDHash{typeIDHash},
			typeIDName{typeIDName},
			mutex{},
			lock{},
			locked{locked}
		{ }

		std::string id; /**< Identifier. */
		ConnectorType type; /**< Type. @see ConnectorType */
		bool active; /**< If connector is active/assigned. */
		std::shared_ptr<void> pointer; /**< Void pointer to contained object. */
		size_t typeIDHash; /**< Hash code of templated type. */
		std::string typeIDName; /**< Type of template instantiation. */
		std::mutex mutex; /**< Mutex to lock resource. */
		std::unique_lock<std::mutex> lock; /**< Lock to use with mutex. */
		bool locked; /**< If connection is locked. */

		ConnectorData(const ConnectorData&) = delete; /**< -Weffc++ */
		ConnectorData& operator=(const ConnectorData&) = delete; /**< -Weffc++ */
	};



	/** Connector map definition. */
	typedef std::map<std::string, std::shared_ptr<ConnectorData>, std::less<std::string>> ConnectorMap;



	/** Connector meta data collector.
	 * This is a workaround so that there can exists one map of connector metadata
	 * for all possible template instatiations of connector that is truly shared
	 * between all of them.
	 */
	struct BVS_PUBLIC ConnectorDataCollector
	{
		/** Map of connectors. */
		static ConnectorMap connectors;
	};
} // namespace BVS



#endif // BVS_CONNECTOR_DATA_H

