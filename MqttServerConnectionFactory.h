//
// Created by dsklyadnev on 5/29/15.
//

#ifndef MQTTBROKER_MQTTSERVERCONNECTIONFACTORY_H
#define MQTTBROKER_MQTTSERVERCONNECTIONFACTORY_H

#include <Poco/Net/TCPServerConnectionFactory.h>

class MqttServerConnectionFactory
    : public Poco::Net::TCPServerConnectionFactory
{
public:
    Poco::Net::TCPServerConnection *createConnection(const Poco::Net::StreamSocket &socket) override;
};


#endif //MQTTBROKER_MQTTSERVERCONNECTIONFACTORY_H
