#ifndef BVSCONNECTOR_H
#define BVSCONNECTOR_H

#include<map>
#include<memory>
#include<string>



// Forward declaration
class BVSConnector;



/** Connector Types.
 * IN  - used for input
 * OUT - used for output
 */
enum class BVSConnectorType { IN, OUT};



/** Connector map definition. */
typedef std::map<std::string, BVSConnector*, std::less<std::string>> BVSConnectorMap;



/** The BVS connection between modules.
 * This class provides access to creating connections between different modules
 * by creating a connector on each side and then pushing data through it like a
 * pipe.
 */
class BVSConnector
{
	public:
		/** Constructs a connector.
		 * @param connectorName The connector's name.
		 * @param connectorType The connector's type.
		 */
		BVSConnector(const std::string& connectorName, BVSConnectorType connectorType);

		// TODO needs to be private, create access functions that also take care of synchronization
		// maybe: get(where to store to) and get returns bool indicating success or failure
		// remember queue design...
		void* data;

		// TODO comments
		template<typename T> void set(T* input);
		template<typename T> T* get();
		
	private:
		std::string identifier; /**< The connector's identifier. */
		BVSConnectorType type; /**< The connector's type. */

		/** Map of connectors, used upon creating to register connector. */
		static BVSConnectorMap connectors;

		BVSConnector(const BVSConnector&); /**< -Weffc++ */
		BVSConnector operator=(const BVSConnector&); /**< -Weffc++ */
		
		friend class BVSLoader;
};



template<typename T> void BVSConnector::set(T* input)
{
	//only for output

	data = input;
}



template<typename T> T* BVSConnector::get()
{
	//only for input

	return static_cast<T*>(data);
}



#endif //BVSCONNECTOR_H

