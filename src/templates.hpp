#include "esp-basic-mqtt.hpp"
// publish templates
template PacketID BasicMqtt::publish<int>(const char* topic, int payload, uint8_t qos, bool retain);
template PacketID BasicMqtt::publish<long>(const char* topic, long payload, uint8_t qos, bool retain);
template PacketID BasicMqtt::publish<long long>(const char* topic, long long payload, uint8_t qos, bool retain);
template PacketID BasicMqtt::publish<int8_t>(const char* topic, int8_t payload, uint8_t qos, bool retain);      // char
template PacketID BasicMqtt::publish<int16_t>(const char* topic, int16_t payload, uint8_t qos, bool retain);    // short
// template PacketID BasicMqtt::publish<int32_t>(const char* topic, int32_t payload, uint8_t qos, bool retain);    // long
// template PacketID BasicMqtt::publish<int64_t>(const char* topic, int64_t payload, uint8_t qos, bool retain);    // long long

template PacketID BasicMqtt::publish<unsigned int>(const char* topic, unsigned int payload, uint8_t qos, bool retain);
template PacketID BasicMqtt::publish<unsigned long>(const char* topic, unsigned long payload, uint8_t qos, bool retain);
template PacketID BasicMqtt::publish<unsigned long long>(const char* topic, unsigned long long payload, uint8_t qos, bool retain);
template PacketID BasicMqtt::publish<uint8_t>(const char* topic, uint8_t payload, uint8_t qos, bool retain);      // unsigned char
template PacketID BasicMqtt::publish<uint16_t>(const char* topic, uint16_t payload, uint8_t qos, bool retain);    // unsigned short
// template PacketID BasicMqtt::publish<u_long>(const char* topic, u_long payload, uint8_t qos, bool retain);        // unsigned long
// template PacketID BasicMqtt::publish<uint32_t>(const char* topic, uint32_t payload, uint8_t qos, bool retain);    // unsigned long
// template PacketID BasicMqtt::publish<uint64_t>(const char* topic, uint64_t payload, uint8_t qos, bool retain);    // unsigned long long
