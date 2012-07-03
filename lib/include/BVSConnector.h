#ifndef BVSCONNECTOR_H
#define BVSCONNECTOR_H

#include<map>
#include<memory>
#include<string>



/** Connector Types.
 * NOOP - not assigned
 * IN   - used for input
 * OUT  - used for output
 */
enum class BVSConnectorType { NOOP, IN, OUT};


struct BVSConnectorData
{
	std::string id;
	BVSConnectorType type;
	bool active;
	void* pointer;
};

/** Connector map definition. */
typedef std::map<std::string, std::shared_ptr<BVSConnectorData>, std::less<std::string>> BVSConnectorMap;

struct BVSConnectorDataCollector
{
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
		 * @param connectorName The connector's name.
		 * @param connectorType The connector's type.
		 */
		BVSConnector(const std::string& connectorName, BVSConnectorType connectorType);

		// TODO comments
		~BVSConnector();

		T* connection;

		T& set();
		const T& get();
		
	private:
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
	
	if (data->type == BVSConnectorType::OUT)
	{
		connection = new T();
		data->pointer = connection;
		data->active = true;
	}
}

template<typename T> BVSConnector<T>::~BVSConnector()
{
	if (data->type == BVSConnectorType::IN) delete connection;
}



template<typename T> T& BVSConnector<T>::set()
{
	// allow set only for output
	if (data->type != BVSConnectorType::OUT)
	{
		//TODO tell to use get if input was desired
		//LOG(0, "trying to read from connector of type != OUT!");
		exit(1);
	}

	return *connection;
}



template<typename T> const T& BVSConnector<T>::get()
{
	// allow get only for output (maybe compiler catches const before, check)
	if (data->type != BVSConnectorType::IN)
	{
		//TODO tell to use set
		//LOG(0, "trying to write to connector of type != IN!");
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

