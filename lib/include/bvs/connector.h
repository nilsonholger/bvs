#ifndef BVS_CONNECTOR_H
#define BVS_CONNECTOR_H

#include <atomic>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <regex>
#include <string>
#include <typeinfo>

#include "bvs/connectordata.h"



/** BVS namespace, contains all library stuff. */
namespace BVS
{
	/** The connection between modules.
	 * This class provides access to creating connections between different modules
	 * by creating a connector on each side and then pushing data through it like a
	 * pipe.
	 *
	 * Do not forget to check your input connectors for valid content, a builtin way
	 * to do so is:
	 * @code
	 * if (!input.receive(object) || !input2.receive(object2) || ...) return BVS::Status::NOINPUT;
	 * @endcode
	 * Obviously, this does NOT guarantee that the input you received will actually
	 * be a valid object, but it guarantees that your programm will not crash due to
	 * non referenceable memory.
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
			 * This copy constructor will use std::move(...) semantics for the
			 * connection as well as data members.
			 * @param[in] t Original.
			 */
			Connector(const Connector& t);

			/** Connector destructor. */
			~Connector();

			/** Connector status check.
			 * Checks whether the connector is active/connected/used.
			 * @return Connector status.
			 */
			bool active();

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
			 * @return Whether the connector was activated.
			 */
			bool activate();

			/** Pointer to the actual object. */
			std::shared_ptr<T> connection;

			/** This connectors metadata. */
			std::shared_ptr<ConnectorData> data;

			Connector operator=(const Connector&) = delete; /**< -Weffc++ */

			/** The Loader needs metadata access. */
			friend class Loader;
	};



	template<typename T> Connector<T>::Connector(const std::string& connectorName, ConnectorType connectorType)
		: connection{nullptr},
		data{std::shared_ptr<ConnectorData>{new ConnectorData{
						connectorName,
						connectorType,
						false,
						nullptr,
						typeid(T).hash_code(),
						typeid(T).name(),
						false}}}
	{
		if (ConnectorDataCollector::connectors.find(connectorName)==ConnectorDataCollector::connectors.end()) {
			if (!std::regex_match(connectorName, std::regex{"[-_[:alnum:]]+"})) {
				std::cerr << "[0|Connector] invalid connector Name, only alphanumeric characters are allowed: " << connectorName << std::endl;
				exit(1);
			}
			ConnectorDataCollector::connectors[connectorName] = data;
		} else {
			std::cerr << "[0|Connector] duplicate name detected: " << connectorName << std::endl;
			exit(1);
		}

		if (data->type == ConnectorType::OUTPUT) {
			connection = std::make_shared<T>();
			data->pointer = connection;
			data->lock = std::unique_lock<std::mutex>{data->mutex, std::defer_lock};
		}
	}



	template<typename T> Connector<T>::Connector(const Connector& t)
		: connection{std::move(t.connection)},
		data{std::move(t.data)}
	{ }



	template<typename T> Connector<T>::~Connector()
	{ }



	template<typename T> bool Connector<T>::active()
	{
		return data->active;
	}



	template<typename T> void Connector<T>::send(const T& t)
	{
		// allow send only for output
		if (data->type != ConnectorType::OUTPUT)
		{
			std::cerr << "[0|Connector] writing to INPUT connector!" << std::endl;
			exit(1);
		}

		if (data->active && !data->locked) data->lock.lock();
		*connection = t;
		if (data->active && !data->locked) data->lock.unlock();
	}



	template<typename T> bool Connector<T>::receive(T& t)
	{
		// allow get only for output (maybe compiler catches const before, check)
		if (data->type != ConnectorType::INPUT)
		{
			std::cerr << "[0|Connector] reading from OUTPUT connector!" << std::endl;
			exit(1);
		}

		if (!data->active && !activate()) return false;

		if (data->active && !data->locked) data->lock.lock();
		t = *connection;
		if (data->active && !data->locked) data->lock.unlock();

		return data->active;
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
		if (data->active)
		{
			data->lock.lock();
			data->locked = true;
		}
	}



	template<typename T> void Connector<T>::unlockConnection()
	{
		if (data->active)
		{
			data->lock.unlock();
			data->locked = false;
		}
	}



	template<typename T> bool Connector<T>::activate()
	{
		if (connection == nullptr && data->pointer != nullptr)
		{
			connection = *reinterpret_cast<std::shared_ptr<T>*>(&data->pointer);
			data->active = true;
			return true;
		}
		else return false;
	}
} // namespace BVS



#endif //BVS_CONNECTOR_H

