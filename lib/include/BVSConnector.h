#ifndef BVSCONNECTOR_H
#define BVSCONNECTOR_H

#include<memory>
#include<string>
#include<vector>



// Forward declaration
class BVS;
class BVSConnector;



// TODO comment
enum class BVSConnectorType { IN, OUT};



// TODO comment
typedef std::vector<BVSConnector> BVSConnectorList;



// TODO comment
class BVSConnector
{
	public:
		BVSConnector(const std::string& identifier, const std::string& connectorName, BVSConnectorType connectorType);
		
	private:
		std::string id;
		std::string name;
		BVSConnectorType type;
		//std::shared_ptr<BVSData> stuff;

		static BVSConnectorList connectors;
		
		friend BVS;
};
#endif //BVSCONNECTOR_H

