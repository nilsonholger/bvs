#include "BVSConnector.h"



std::vector<BVSConnector> BVSConnector::connectors;

BVSConnector::BVSConnector(const std::string& identifier, const std::string& connectorName, BVSConnectorType connectorType)
	: id(identifier)
	, name(connectorName)
	, type(connectorType)
{
	connectors.push_back(*this);
}

