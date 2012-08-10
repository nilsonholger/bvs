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
			/** Constructs a Connector.
			 * @param[in] connectorName The connector's name.
			 * @param[in] connectorType The connector's type.
			 * @see ConnectorType
			 */
			Connector(const std::string& connectorName, ConnectorType connectorType);

			/** Connector destructor. */
			~Connector();

			/** Write to output.
			 * @param[in] t The object you want to send.
			 */
			void send(const T& t);

			/** Read from input.
			 * @param[out] t Object to receive into.
			 * @return True if successfully retrieved an object, false otherwise.
			 */
			bool receive(T& t);

			/** Access operator*.
			 * Use this operator to write/read data to/from the connection.
			 * This is especially usefull if you have more complicated objects
			 * that you want to build incrementally or set some flags or
			 * metadata on because you have direct access.
			 * Caution in multithreaded scenarios, you have to lock the
			 * object while using it, or another thread could pick up an
			 * incomplete object at any time.
			 */
			T& operator*();

			/** Locks the connection object. */
			void lockConnection();

			/** Unlocks the connection object. */
			void unlockConnection();

		private:
			/** Pointer to the actual object. */
			T* connection;

			/** This connectors metadata. */
			std::shared_ptr<ConnectorData> data;

			Connector(const Connector&) = delete; /**< -Weffc++ */
			Connector operator=(const Connector&) = delete; /**< -Weffc++ */

			friend class Loader;
	};



	template<typename T> Connector<T>::Connector(const std::string& connectorName, ConnectorType connectorType)
		: connection(nullptr)
		, data(std::shared_ptr<ConnectorData>(new ConnectorData(connectorName,
						connectorType, false, nullptr, typeid(T).hash_code(),
						typeid(T).name(), nullptr, false)))
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



	template<typename T> void Connector<T>::send(const T& t)
	{
		// allow send only for output
		if (data->type != ConnectorType::OUTPUT)
		{
			std::cerr << "[0|Connector] trying to read from connector of type != OUTPUT!" << std::endl;
			exit(1);
		}

		if (!data->locked) data->mutex->lock();
		*connection = t;
		if (!data->locked) data->mutex->unlock();
	}



	template<typename T> bool Connector<T>::receive(T& t)
	{
		// allow get only for output (maybe compiler catches const before, check)
		if (data->type != ConnectorType::INPUT)
		{
			std::cerr << "trying to write to connector of type != INPUT!" << std::endl;
			exit(1);
		}

		// check status
		if (!data->active) return false;

		// check initilization
		if (connection == nullptr && data->pointer != nullptr && data->active)
		{
			connection = static_cast<T*>(data->pointer);
			data->active = true;
		}

		if (!data->locked) data->mutex->lock();
		t = *connection;
		if (!data->locked) data->mutex->unlock();

		return true;
	}



	template<typename T> T& Connector<T>::operator*()
	{
		// check initiliziation
		if (connection == nullptr && data->pointer != nullptr && data->active)
		{
			connection = static_cast<T*>(data->pointer);
			data->active = true;
		}

		return *connection;
	}



	template<typename T> void Connector<T>::lockConnection()
	{
		data->mutex->lock();
		data->locked = true;
	}



	template<typename T> void Connector<T>::unlockConnection()
	{
		data->mutex->unlock();
		data->locked = false;
	}
} // namespace BVS



#endif //BVS_CONNECTOR_H

