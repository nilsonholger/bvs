#ifndef BVS_CONNECTORDATA_H
#define BV__CONNECTORDATA_H

#include<mutex>
#include<string>



/** BVS namespace, contains all library stuff. */
namespace BVS
{
	/** Connector Types.
	 * NOOP - not assigned
	 * INPUTPUT   - used for input
	 * OUTPUTPUT  - used for output
	 */
	enum class ConnectorType { NOOP, INPUT, OUTPUT};



	/** Connector meta data store. */
	struct ConnectorData
	{
		std::string id; /**< Identifier. */
		ConnectorType type; /**< Type. @see ConnectorType */
		bool active; /**< If connector is active/assigned. */
		void* pointer; /**< Void pointer to contained object. */
		size_t typeIDHash; /**< Hash code of templated type. */
		std::string typeIDName; /**< Type of template instantiation. */
		std::mutex* mutex; /**< Mutex to lock resource. */
	};



	/** Connector map definition. */
	typedef std::map<std::string, std::shared_ptr<ConnectorData>, std::less<std::string>> ConnectorMap;



	/** Connector meta data collector.
	 * This is a workaround so that there can exists one map of connector metadata
	 * for all possible template instatiations of connector that is truly shared
	 * between all of them.
	 */
	struct ConnectorDataCollector
	{
		/** Map of connectors. */
		static ConnectorMap connectors;
	};
} // namespace BVS


#endif // BVS_CONNECTOR_DATA_H

