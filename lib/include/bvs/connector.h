#ifndef BVS_CONNECTOR_H
#define BVS_CONNECTOR_H

#include<iostream>
#include<map>
#include<memory>
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
		void* pointer; /** Void pointer to contained object. */
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



	/** The connection between modules.
	 * This class provides access to creating connections between different modules
	 * by creating a connector on each side and then pushing data through it like a
	 * pipe.
	 */
	template<typename T> class Connector
	{
		public:
			/** Constructs a connector.
			 * @param[in] connectorName The connector's name.
			 * @param[in] connectorType The connector's type.
			 */
			Connector(const std::string& connectorName, ConnectorType connectorType);

			/** Connector destructor. */
			~Connector();

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
			std::shared_ptr<ConnectorData> data;

			Connector(const Connector&); /**< -Weffc++ */
			Connector operator=(const Connector&); /**< -Weffc++ */

			friend class Loader;
	};



	template<typename T> Connector<T>::Connector(const std::string& connectorName, ConnectorType connectorType)
		: connection(nullptr)
		  , data(std::shared_ptr<ConnectorData>(new ConnectorData{connectorName, connectorType, false, nullptr}))
	{
		ConnectorDataCollector::connectors[connectorName] = data;

		if (data->type == ConnectorType::OUTPUT)
		{
			connection = new T();
			data->pointer = connection;
			data->active = true;
		}
	}

	template<typename T> Connector<T>::~Connector()
	{
		if (data->type == ConnectorType::OUTPUT) delete connection;
	}



	template<typename T> T& Connector<T>::set()
	{
		// allow set only for output
		if (data->type != ConnectorType::OUTPUT)
		{
			std::cerr << "[0|Connector] trying to read from connector of type != OUTPUT!" << std::endl;
			exit(1);
		}

		return *connection;
	}



	template<typename T> const T& Connector<T>::get()
	{
		// allow get only for output (maybe compiler catches const before, check)
		if (data->type != ConnectorType::INPUT)
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
} // namespace BVS



#endif //BVS_CONNECTOR_H

