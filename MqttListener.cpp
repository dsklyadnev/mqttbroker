//
// Created by dsklyadnev on 5/29/15.
//

#include "MqttListener.h"
#include "MqttServerConnectionFactory.h"

const char *MqttListener::name() const
{
    return "MqttListener";
}

void MqttListener::initialize(Poco::Util::Application & app)
{
    uint16_t port = 1890;//config().getInt("Listener.Port");
    serverSocket.reset(new Poco::Net::ServerSocket(port));
    tcpServer.reset(new Poco::Net::TCPServer(new MqttServerConnectionFactory, *serverSocket));
    tcpServer->start();
}

void MqttListener::uninitialize()
{
    tcpServer->stop();
}
