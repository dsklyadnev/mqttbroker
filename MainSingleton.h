//
// Created by dsklyadnev on 6/2/15.
//

#ifndef MQTTBROKER_MAINSINGLETON_H
#define MQTTBROKER_MAINSINGLETON_H

#include "Singleton.h"

#include <string>
#include <vector>

struct Subscriber
{
    //
};

struct Message
{
    std::string topic;
    uint8_t qos;
    std::vector <uint8_t> payload;
    Message() = default;
    Message(const std::string & topic, uint8_t qos, std::vector <uint8_t> & payload)
        : topic(topic), qos(qos), payload(payload)
    {}
};

class MainSingleton
    : public Singleton <MainSingleton>
{
public:
    void RegisterSubscriber();
    void PushMessageToQueue(const Message & message);
private:
    MainSingleton() = default;
    friend class Singleton <MainSingleton>;
};


#endif //MQTTBROKER_MAINSINGLETON_H
