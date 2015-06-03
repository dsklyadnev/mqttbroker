//
// Created by dsklyadnev on 5/29/15.
//

#ifndef MQTTBROKER_MQTTSERVERCONNECTION_H
#define MQTTBROKER_MQTTSERVERCONNECTION_H

#include <Poco/Net/TCPServerConnection.h>

#include <vector>

class MqttServerConnection
    : public Poco::Net::TCPServerConnection
{
public:
    MqttServerConnection(const Poco::Net::StreamSocket & streamSocket);
private:
    struct MqttSubscriptionTopic
    {
        std::string topic;
        uint8_t qos = 0;
        bool failure = false;
        MqttSubscriptionTopic() = default;
        MqttSubscriptionTopic(const std::string & topic, uint8_t qos)
            : topic(topic, qos)
        {}
    };
    /// Session description
    struct MqttSession
    {
        /// Protocol name
        std::string protocolName;
        /// Keep Alive time
        uint16_t keepAlive;
        /// Client ID
        std::string clientId;
        /// User name
        std::string userName;
        /// Password
        std::vector <uint8_t> password;
        /// Will Topic
        std::string willTopic;
        /// Will Message
        std::string willMessage;
        /// Subscription topics
        std::vector <MqttSubscriptionTopic> subscriptionTopics;
    };
    MqttSession session;
    virtual void run() override;
private:
    //void formCONNACK(std::vector <uint8_t> & data, Mqtt::ConnectReturnCode returnCode, bool sessionPresent) const;
    void formSUBACK(std::vector <uint8_t> & data, uint16_t packetIdentifier) const;
};


#endif //MQTTBROKER_MQTTSERVERCONNECTION_H
