//
// Created by dsklyadnev on 5/29/15.
//

#include "MqttServerConnection.h"

#include <stdint.h>
#include <Poco/Util/Application.h>
#include <Poco/ByteOrder.h>

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
            bool    reserved     : 1;
            bool    cleanSession : 1;
            bool    willFlag     : 1;
            uint8_t willQos      : 2;
            bool    willRetain   : 1;
            bool    passwordFlag : 1;
            bool    userNameFlag : 1;
        } connectFlags;
        uint16_t keepAlive = 0;
    };

    enum class ConnectReturnCode : uint8_t
    {
        ConnectionAccepted           = 0x00, ///< Connection accepted
        UnacceptableProtocolVersion  = 0x01, ///< The Server does not support the level of the MQTT protocol requested by the Client
        IdentifierRejected           = 0x02, ///< The Client identifier is correct UTF-8 but not mqtt-v3.1.1-os 29 October 2014 Standards Track Work Product Copyright © OASIS Open 2014. All Rights Reserved. Page 33 of 81 allowed by the Server
        ServerUnavailable            = 0x03, ///< The Network Connection has been made but the MQTT service is unavailable
        BadAuthData                  = 0x04, ///< Bad user name or password The data in the user name or password is malformed
        NotAuthorized                = 0x05  ///<The Client is not authorized to connect
                                              // 6-255 Reserved for future use
    };

    struct ConnackFrameVariableHeader
    {
        bool sessionPresent = false;
        ConnectReturnCode connectReturnCode;
    };

    enum class SubackReturnCode : uint8_t
    {
        SuccessMaximumQos0 = 0x00,
        SuccessMaximumQos1 = 0x01,
        SuccessMaximumQos2 = 0x02,
        Failure            = 0x80
    };

    struct PublishFlags
    {
        bool    dup    = false;
        uint8_t qos    = 0;
        bool    retain = false;
    };
}

MqttServerConnection::MqttServerConnection(const Poco::Net::StreamSocket &streamSocket)
    : Poco::Net::TCPServerConnection(streamSocket)
{
}

struct SubTopic
{
    std::string topic;
    uint8_t qos = 0;
    SubTopic() = default;
    SubTopic(const std::string & topic, uint8_t qos)
        : topic(topic, qos)
    {}
};

void MqttServerConnection::run()
{
    std::vector<uint8_t>data(1024);

    while(true)
    {
        data.resize(1024);
        socket().receiveBytes(data.data(), 2);
        auto fixedHeader = reinterpret_cast<Mqtt::FrameFixedHeader *>(data.data());
        socket().receiveBytes(data.data() + 2, fixedHeader->length);

        uint8_t *ptr = data.data() + 2;
        switch ((uint8_t) fixedHeader->controlPackerType)
        {
            case (uint8_t) Mqtt::ControlPacketType::CONNECT:
            {
                uint16_t protocolNameLen = Poco::ByteOrder::toBigEndian(*reinterpret_cast<uint16_t *>(ptr));
                ptr += 2;
                std::string protocolName(reinterpret_cast<char *>(ptr), protocolNameLen);
                ptr += protocolNameLen;
                auto variableHeader = *reinterpret_cast<Mqtt::ConnectFrameVariableHeader *>(ptr);
                variableHeader.keepAlive = Poco::ByteOrder::toBigEndian(variableHeader.keepAlive);
                ptr += sizeof(Mqtt::ConnectFrameVariableHeader);
                uint16_t clientIdLen = Poco::ByteOrder::toBigEndian(*reinterpret_cast<uint16_t *>(ptr));
                ptr += 2;
                std::string clientId(reinterpret_cast<char *>(ptr), clientIdLen);
                ptr += clientIdLen;
                if (variableHeader.connectFlags.willFlag)
                {
                    uint16_t willTopicLen = Poco::ByteOrder::fromBigEndian(*reinterpret_cast<uint16_t *>(ptr));
                    ptr += 2;
                    std::string willTopic(reinterpret_cast<char *>(ptr), willTopicLen);
                    ptr += willTopicLen;
                    uint16_t willMessageLen = Poco::ByteOrder::fromBigEndian(*reinterpret_cast<uint16_t *>(ptr));
                    ptr += 2;
                    std::string willMessage(reinterpret_cast<char *>(ptr), willMessageLen);
                    ptr += willMessageLen;
                }
                if (variableHeader.connectFlags.userNameFlag)
                {
                    uint16_t userNameLen = Poco::ByteOrder::fromBigEndian(*reinterpret_cast<uint16_t *>(ptr));
                    ptr += 2;
                    std::string userName(reinterpret_cast<char *>(ptr), userNameLen);
                    ptr += userNameLen;
                }
                if (variableHeader.connectFlags.passwordFlag)
                {
                    uint16_t passwordLen = Poco::ByteOrder::fromBigEndian(*reinterpret_cast<uint16_t *>(ptr));
                    ptr += 2;
                    std::vector<uint8_t> password(passwordLen);
                    memcpy(password.data(), ptr, passwordLen);
                    ptr += passwordLen;
                }
                {// Form CONNACK
                    data.resize(sizeof(Mqtt::FrameFixedHeader) + sizeof(Mqtt::ConnackFrameVariableHeader));
                    fixedHeader->controlPackerType = Mqtt::ControlPacketType::CONNACK;
                    fixedHeader->flags = 0;
                    fixedHeader->length = 2;
                    ptr = data.data() + sizeof(Mqtt::FrameFixedHeader);
                    Mqtt::ConnackFrameVariableHeader *connackFrameVariableHeader = reinterpret_cast<Mqtt::ConnackFrameVariableHeader *>(ptr);
                    connackFrameVariableHeader->connectReturnCode = Mqtt::ConnectReturnCode::ConnectionAccepted;
                    connackFrameVariableHeader->sessionPresent = false;
                    socket().sendBytes(data.data(), data.size());
                }
            }
            break;
            case (uint8_t) Mqtt::ControlPacketType::SUBSCRIBE:
            {
                uint16_t packetIdentifier = Poco::ByteOrder::fromBigEndian(*reinterpret_cast<uint16_t *>(ptr));
                ptr += 2;
                std::vector <SubTopic> topics;
                while (ptr - data.data() - 2 < fixedHeader->length)
                {
                    uint16_t topicLen = Poco::ByteOrder::fromBigEndian(*reinterpret_cast<uint16_t *>(ptr));
                    ptr += 2;
                    std::string topic(reinterpret_cast<char *>(ptr), topicLen);
                    ptr += topicLen;
                    uint8_t qos = *ptr;
                    ptr++;
                    topics.push_back({topic, qos});
                }
                { // Form SUBACK
                    data.resize(sizeof(Mqtt::FrameFixedHeader) + 2 + topics.size());
                    fixedHeader->controlPackerType = Mqtt::ControlPacketType::SUBACK;
                    fixedHeader->flags = 0;
                    fixedHeader->length = 2 + topics.size();
                    ptr = data.data() + sizeof(Mqtt::FrameFixedHeader);
                    uint16_t * subackPackId = reinterpret_cast<uint16_t*>(ptr);
                    *subackPackId = Poco::ByteOrder::toBigEndian(packetIdentifier);
                    ptr += 2;
                    for (int i = 0; i < topics.size(); ++i)
                    {
                        *ptr = static_cast<uint8_t>(Mqtt::SubackReturnCode::SuccessMaximumQos2);
                        ptr++;
                    }
                    socket().sendBytes(data.data(), data.size());
                }
            }
            break;
            case (uint8_t) Mqtt::ControlPacketType::UNSUBSCRIBE:
            {
                uint16_t packetIdentifier = Poco::ByteOrder::fromBigEndian(*reinterpret_cast<uint16_t *>(ptr));
                ptr += 2;
                //...
            }
            break;
            case (uint8_t) Mqtt::ControlPacketType::PUBLISH:
            {
                uint16_t packetIdentifier = 0;
                Mqtt::PublishFlags publishFlags;
                publishFlags.retain = fixedHeader->flags & 1;
                publishFlags.qos    = (fixedHeader->flags & 6) >> 1;
                publishFlags.dup    = !!fixedHeader->flags & 8;
                uint16_t topicLen = Poco::ByteOrder::fromBigEndian(*reinterpret_cast<uint16_t *>(ptr));
                ptr += 2;
                std::string topic(reinterpret_cast<char *>(ptr), topicLen);
                ptr += topicLen;
                if (publishFlags.qos > 0)
                {
                    packetIdentifier = Poco::ByteOrder::fromBigEndian(*reinterpret_cast<uint16_t *>(ptr));
                    ptr += 2;
                }
                size_t payloadLen = fixedHeader->length - topic.size() - (publishFlags.qos > 0 ? 2 : 0);
                std::vector <uint8_t> payload(payloadLen);
                memcpy(payload.data(), ptr, payloadLen);
                if (publishFlags.qos == 1)
                { // Form PUBACK
                    data.resize(sizeof(Mqtt::FrameFixedHeader) + 2);
                    fixedHeader->controlPackerType = Mqtt::ControlPacketType::PUBACK;
                    fixedHeader->flags = 0;
                    fixedHeader->length = 2;
                    ptr = data.data() + sizeof(Mqtt::FrameFixedHeader);
                    uint16_t * subackPackId = reinterpret_cast<uint16_t*>(ptr);
                    *subackPackId = Poco::ByteOrder::toBigEndian(packetIdentifier);
                    socket().sendBytes(data.data(), data.size());
                }
            }
        }

        Poco::Util::Application &app = Poco::Util::Application::instance();
        app.logger().debug("run");
    }
}
