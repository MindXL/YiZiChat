#pragma once

namespace YiZi::Packet
{
    enum class PacketType
    {
        TestRequest, TestResponse
    };

    struct PacketHeader
    {
        uint8_t type;
        uint32_t length;
    };

    static constexpr int PACKET_HEADER_LENGTH = sizeof(PacketHeader);

    struct TestRequest
    {
        uint32_t length;
        uint8_t message[30];
    };

    typedef TestRequest TestResponse;

    static constexpr int TEST_REQUEST_LENGTH = sizeof(TestRequest);
    static constexpr int TEST_RESPONSE_LENGTH = sizeof(TestResponse);

    constexpr int REQUEST_MAX_LENGTH()
    {
        /*
        return PACKET_HEADER_LENGTH + std::max({
                   TEST_REQUEST_LENGTH,
                   0
               });
        */
        return PACKET_HEADER_LENGTH + TEST_REQUEST_LENGTH;
    }

    constexpr int RESPONSE_MAX_LENGTH()
    {
        /*
        return PACKET_HEADER_LENGTH + std::max({
                   TEST_RESPONSE_LENGTH,
                   0
               });
        */
        return PACKET_HEADER_LENGTH + TEST_RESPONSE_LENGTH;
    }
}
