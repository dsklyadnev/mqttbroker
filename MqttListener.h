//
// Created by dsklyadnev on 5/29/15.
//

#ifndef MQTTBROKER_MQTTLISTENER_H
#define MQTTBROKER_MQTTLISTENER_H

#include <Poco/Util/Subsystem.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/TCPServer.h>

#include <memory>

class MqttListener
    : public Poco::Util::Subsystem
{
public:
    MqttListener() = default;

    const char* name() const override;
private:
    void initialize(Poco::Util::Application& app) override;
    void uninitialize() override;
private:
    std::shared_ptr <Poco::Net::ServerSocket> serverSocket;
    std::shared_ptr <Poco::Net::TCPServer> tcpServer;
};


#endif //MQTTBROKER_MQTTLISTENER_H
