#include "BVSConnector.h"



std::vector<BVSConnector> BVSConnector::connectors;

BVSConnector::BVSConnector(const std::string& connectorName, BVSConnectorType connectorType)
	: identifier(connectorName)
	, type(connectorType)
{
	connectors.push_back(*this);
}

