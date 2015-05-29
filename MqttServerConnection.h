//
// Created by dsklyadnev on 5/29/15.
//

#ifndef MQTTBROKER_MQTTSERVERCONNECTION_H
#define MQTTBROKER_MQTTSERVERCONNECTION_H

#include <Poco/Net/TCPServerConnection.h>

class MqttServerConnection
    : public Poco::Net::TCPServerConnection
{
public:
    MqttServerConnection(const Poco::Net::StreamSocket & streamSocket);
private:
    virtual void run() override;
};


#endif //MQTTBROKER_MQTTSERVERCONNECTION_H
