#ifndef BVSCONNECTOR_H
#define BVSCONNECTOR_H

#include<memory>
#include<string>
#include<vector>



// Forward declaration
class BVSConnector;



// TODO comment
enum class BVSConnectorType { IN, OUT};



// TODO comment
typedef std::vector<BVSConnector> BVSConnectorList;



// TODO comment
class BVSConnector
{
	public:
		BVSConnector(const std::string& connectorName, BVSConnectorType connectorType);
		
	private:
		std::string identifier;
		BVSConnectorType type;

		static BVSConnectorList connectors;
		
		friend class BVSLoader;
};
#endif //BVSCONNECTOR_H

