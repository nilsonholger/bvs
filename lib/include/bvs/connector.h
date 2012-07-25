#ifndef BVS_CONNECTOR_H
#define BVS_CONNECTOR_H

#include<iostream>
#include<map>
#include<memory>
#include<mutex>
#include<string>
#include<typeinfo>

#include "bvs/connectordata.h"



/** BVS namespace, contains all library stuff. */
namespace BVS
{
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
			 * @see ConnectorType
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

			/** Access operator*.
			 * Use this operator to write/read data to/from the connection.
			 * TODO read/write explanation
			 */
			T& operator*();

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
		, data(std::shared_ptr<ConnectorData>(new ConnectorData{connectorName, connectorType, false, nullptr, typeid(T).hash_code(), typeid(T).name(), nullptr}))
	{
		ConnectorDataCollector::connectors[connectorName] = data;

		if (data->type == ConnectorType::OUTPUT)
		{
			connection = new T();
			data->pointer = connection;
			data->active = true;
			data->mutex = new std::mutex();
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



	template<typename T> T& Connector<T>::operator*()
	{
		std::lock_guard<std::mutex> lock(*data->mutex);

		// check initiliziation
		if (connection == nullptr && data->pointer != nullptr && data->active)
		{
			connection = static_cast<T*>(data->pointer);
			data->active = true;
		}

		return *connection;
	}
} // namespace BVS



#endif //BVS_CONNECTOR_H

