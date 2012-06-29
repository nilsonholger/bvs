#include "BVSConnector.h"



BVSConnectorMap BVSConnector::connectors;

BVSConnector::BVSConnector(const std::string& connectorName, BVSConnectorType connectorType)
	: data()
	, id(connectorName)
	, type(connectorType)
{
	connectors[id] = this;
}

