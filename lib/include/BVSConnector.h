#ifndef BVSCONNECTOR_H
#define BVSCONNECTOR_H

#include<map>
#include<memory>
#include<string>



// Forward declaration
class BVSConnector;



// TODO comment
enum class BVSConnectorType { IN, OUT};



// TODO comment
typedef std::map<std::string, BVSConnector*, std::less<std::string>> BVSConnectorMap;



// TODO comment
class BVSConnector
{
	public:
		BVSConnector(const std::string& connectorName, BVSConnectorType connectorType);
		std::shared_ptr<int> data;
		
	private:
		std::string identifier;
		BVSConnectorType type;

		static BVSConnectorMap connectors;
		
		friend class BVSLoader;
};
#endif //BVSCONNECTOR_H

