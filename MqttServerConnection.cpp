//
// Created by dsklyadnev on 5/29/15.
//

#include "MqttServerConnection.h"

#include <stdint.h>
#include <Poco/Util/Application.h>

namespace Mqtt
{
    /// Control packet types are described in this enum
    enum class ControlPacketType : uint8_t
    {
        CONNECT     = 1, ///< Client request to connect to Server
        CONNACK     = 2, ///< Connect acknowledgment
        PUBLISH     = 3, ///< Publish message
        PUBACK      = 4, ///< Publish acknowledgment
        PUBREC      = 5, ///< Publish received (assured delivery part 1)
        PUBREL      = 6, ///< Publish release (assured delivery part 2)
        PUBCOMP     = 7, ///< Publish complete (assured delivery part 3)
        SUBSCRIBE   = 8, ///< Client subscribe request
        SUBACK      = 9,  ///< Subscribe acknowledgment
        UNSUBSCRIBE = 10, ///< Client to Server Unsubscribe request
        UNSUBACK    = 11, ///< Server to Client Unsubscribe acknowledgment
        PINGREQ     = 12, ///< Client to Server PING request
        PINGRESP    = 13, ///< Server to Client PING response
        DISCONNECT  = 14  ///< Client to Server Client is disconnecting
    };

    /// Control frame fixed part
    struct FrameFixedHeader
    {
        uint8_t flags : 4;
        ControlPacketType controlPackerType : 4;
        uint8_t length = 0;

        FrameFixedHeader() = default;

        FrameFixedHeader(ControlPacketType controlPackerType, uint8_t flags, uint8_t length)
        : flags(flags), controlPackerType(controlPackerType), length(length)
        {
        }

        operator std::vector<uint8_t>()
        {
            std::vector<uint8_t> result(sizeof(Mqtt::FrameFixedHeader), 0);
            memcpy(result.data(), this, sizeof(Mqtt::FrameFixedHeader));
            return result;
        }
    };

    struct ConnectFrameVariableHeader
    {
        //std::string protocol;
        uint8_t protocolLevel = 0;
        struct ConnectFlags
        {
            bool reserved : 1;
            bool cleanSession : 1;
            bool willFlag : 1;
            bool willQos : 1;
            bool willRetain : 1;
            bool passwordFlag : 1;
            bool userNameFlag : 1;
        } connectFlags;
        uint16_t keepAlive = 0;
    };
}

MqttServerConnection::MqttServerConnection(const Poco::Net::StreamSocket &streamSocket)
    : Poco::Net::TCPServerConnection(streamSocket)
{
}

void MqttServerConnection::run()
{
    std::vector<uint8_t>data(1024);
    socket().receiveBytes(data.data(), 2);
    auto fixedHeader = reinterpret_cast<Mqtt::FrameFixedHeader*>(data.data());
    socket().receiveBytes(data.data()+2, fixedHeader->length);

    uint8_t * ptr = data.data() + 2;
    switch ((uint8_t)fixedHeader->controlPackerType)
    {
        case (uint8_t)Mqtt::ControlPacketType::CONNECT:
        {
            uint16_t protocolNameLen = *reinterpret_cast<uint16_t*>(ptr);
            ptr+=2;
            std::string protocolName(reinterpret_cast<char*>(ptr + protocolNameLen), protocolNameLen);
            ptr+=protocolNameLen;
            auto variableHeader = reinterpret_cast<Mqtt::ConnectFrameVariableHeader*>(ptr);
            ptr += sizeof(Mqtt::ConnectFrameVariableHeader);
        };
    }

    Poco::Util::Application & app = Poco::Util::Application::instance();
    app.logger().debug("run");
}
