//
// Created by dsklyadnev on 5/29/15.
//

#include "MqttServerConnectionFactory.h"
#include "MqttServerConnection.h"

Poco::Net::TCPServerConnection *MqttServerConnectionFactory::createConnection(const Poco::Net::StreamSocket &socket)
{
    return new MqttServerConnection(socket);
}
