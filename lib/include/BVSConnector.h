#ifndef BVSCONNECTOR_H
#define BVSCONNECTOR_H

#include<iostream>
#include<map>
#include<memory>
#include<string>



/** Connector Types.
 * NOOP - not assigned
 * INPUTPUT   - used for input
 * OUTPUTPUT  - used for output
 */
enum class BVSConnectorType { NOOP, INPUT, OUTPUT};



/** BVS connector meta data store. */
struct BVSConnectorData
{
	std::string id; /**< Identifier. */
	BVSConnectorType type; /**< Type. @see BVSConnectorType */
	bool active; /**< If connector is active/assigned. */
	void* pointer; /** Void pointer to contained object. */
};



/** Connector map definition. */
typedef std::map<std::string, std::shared_ptr<BVSConnectorData>, std::less<std::string>> BVSConnectorMap;



/** BVS connector meta data collector.
 * This is a workaround so that there can exists one map of connector metadata
 * for all possible template instatiations of BVSConnector that is truly shared
 * between all of them.
 */
struct BVSConnectorDataCollector
{
	/** Map of connectors. */
	static BVSConnectorMap connectors;
};



/** The BVS connection between modules.
 * This class provides access to creating connections between different modules
 * by creating a connector on each side and then pushing data through it like a
 * pipe.
 */
template<typename T> class BVSConnector
{
	public:
		/** Constructs a connector.
		 * @param[in] connectorName The connector's name.
		 * @param[in] connectorType The connector's type.
		 */
		BVSConnector(const std::string& connectorName, BVSConnectorType connectorType);

		/** Connector destructor. */
		~BVSConnector();

		/** Write to output.
		 * @return Reference to connection object.
		 */
		T& set();

		/** Read from input.
		 * @return Constant reference to connection object.
		 */
		const T& get();
		
	private:
		/** Pointer to the actual object. */
		T* connection;

		/** This connectors metadata. */
		std::shared_ptr<BVSConnectorData> data;

		BVSConnector(const BVSConnector&); /**< -Weffc++ */
		BVSConnector operator=(const BVSConnector&); /**< -Weffc++ */
		
		friend class BVSLoader;
};



template<typename T> BVSConnector<T>::BVSConnector(const std::string& connectorName, BVSConnectorType connectorType)
	: connection(nullptr)
	, data(std::shared_ptr<BVSConnectorData>(new BVSConnectorData{connectorName, connectorType, false, nullptr}))
{
	BVSConnectorDataCollector::connectors[connectorName] = data;
	
	if (data->type == BVSConnectorType::OUTPUT)
	{
		connection = new T();
		data->pointer = connection;
		data->active = true;
	}
}

template<typename T> BVSConnector<T>::~BVSConnector()
{
	if (data->type == BVSConnectorType::OUTPUT) delete connection;
}



template<typename T> T& BVSConnector<T>::set()
{
	// allow set only for output
	if (data->type != BVSConnectorType::OUTPUT)
	{
		std::cerr << "[0|BVSConnector] trying to read from connector of type != OUTPUT!" << std::endl;
		exit(1);
	}

	return *connection;
}



template<typename T> const T& BVSConnector<T>::get()
{
	// allow get only for output (maybe compiler catches const before, check)
	if (data->type != BVSConnectorType::INPUT)
	{
		std::cerr << "trying to write to connector of type != INPUT!" << std::endl;
		exit(1);
	}

	if (connection == nullptr && data->pointer != nullptr && data->active)
	{
		connection = static_cast<T*>(data->pointer);
		data->active = true;
	}

	return *connection;
}



#endif //BVSCONNECTOR_H

