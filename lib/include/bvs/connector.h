#ifndef BVS_CONNECTOR_H
#define BVS_CONNECTOR_H

#include<atomic>
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
			 * Dependencies: any used type must provide default constructor T()
			 * @param[in] connectorName The connector's name.
			 * @param[in] connectorType The connector's type.
			 * @see ConnectorType
			 */
			Connector(const std::string& connectorName, ConnectorType connectorType);

			/** Copy constructor.
			 * @param[in] t Original.
			 */
			Connector(const Connector& t);

			/** Connector destructor. */
			~Connector();

			/** Write to output.
			 * @param[in] t The object you want to send.
			 */
			void send(const T& t);

			/** Read from input.
			 * @param[out] t Object to receive into.
			 * @return True if input is connected to an output, false otherwise.
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

			/** Access operator->.
			 * @see operator*()
			 */
			T* operator->();

			/** Locks the connection object. */
			void lockConnection();

			/** Unlocks the connection object. */
			void unlockConnection();

		private:
			/** Activate connector.
			 * This means that an input is actually connected to an output enabling
			 * data to be retrieved. Before this executes, the connector will only
			 * contain a T() object and not the actual data from an output.
			 */
			void activate();

			/** Pointer to the actual object. */
			std::shared_ptr<T> connection;

			/** This connectors metadata. */
			std::shared_ptr<ConnectorData> data;

			Connector operator=(const Connector&) = delete; /**< -Weffc++ */

			friend class Loader;
	};



	template<typename T> Connector<T>::Connector(const std::string& connectorName, ConnectorType connectorType)
		: connection(nullptr)
		, data(std::shared_ptr<ConnectorData>(new ConnectorData(
						connectorName,
						connectorType,
						false,
						nullptr,
						typeid(T).hash_code(),
						typeid(T).name(),
						nullptr,
						false)))
	{
		ConnectorDataCollector::connectors[connectorName] = data;

		if (data->type == ConnectorType::OUTPUT)
		{
			connection = std::make_shared<T>();
			data->pointer = connection;
			data->active = true;
			data->mutex = new std::mutex();
		}
	}



	template<typename T> Connector<T>::Connector(const Connector& t)
		: connection(t.connection)
		, data(t.data)
	{ }



	template<typename T> Connector<T>::~Connector()
	{ }



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

		if (!data->active) activate();

		if (!data->locked) data->mutex->lock();
		t = *connection;
		if (!data->locked) data->mutex->unlock();

		return true;
	}



	template<typename T> T& Connector<T>::operator*()
	{
		if (!data->active) activate();

		return *connection;
	}



	template<typename T> T* Connector<T>::operator->()
	{
		if (!data->active) activate();

		return &(*connection);
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



	template<typename T> void Connector<T>::activate()
	{
		if (connection == nullptr && data->pointer != nullptr)
		{
			connection = *reinterpret_cast<std::shared_ptr<T>*>(&data->pointer);
			data->active = true;
		}
	}
} // namespace BVS



#endif //BVS_CONNECTOR_H

