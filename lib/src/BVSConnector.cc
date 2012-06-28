#include "BVSConnector.h"



BVSConnectorMap BVSConnector::connectors;

BVSConnector::BVSConnector(const std::string& connectorName, BVSConnectorType connectorType)
	: data()
	, identifier(connectorName)
	, type(connectorType)
{
	connectors[identifier] = this;
}

