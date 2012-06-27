#include "BVSConnector.h"



BVSConnectorMap BVSConnector::connectors;

BVSConnector::BVSConnector(const std::string& connectorName, BVSConnectorType connectorType)
	: data(new int)
	, identifier(connectorName)
	, type(connectorType)
{
	connectors[identifier] = this;
}

