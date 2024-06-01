#pragma once

#include <Arduino.h>
#include <AsyncMqttClient.h>
#include <Ticker.h>
#include <functional>
#include <stdint.h>
#include <string>
#include <vector>

// #define BASIC_MQTT_DEBUG
// debug printing macros
// clang-format off
#ifdef BASIC_MQTT_DEBUG
#define DEBUG_PRINTER Serial
#define BASIC_MQTT_PRINT(...) { DEBUG_PRINTER.print(__VA_ARGS__); }
#define BASIC_MQTT_PRINTLN(...) { DEBUG_PRINTER.println(__VA_ARGS__); }
#define BASIC_MQTT_PRINTF(...) { DEBUG_PRINTER.printf(__VA_ARGS__); }
#else
#define BASIC_MQTT_PRINT(...) {}
#define BASIC_MQTT_PRINTLN(...) {}
#define BASIC_MQTT_PRINTF(...) {}
#endif
// clang-format on

#define MQTT_BLINK_ON 100
#define MQTT_BLINK_OFF 150
#define MQTT_NO_BLINK MQTT_BLINK_ON + MQTT_BLINK_OFF
#define MQTT_MANUAL_RECONNECT_DELAY 10
#define MQTT_AUTO_RECONNECT_DELAY 30
#define STATUS_ON_MSG "1"
#define STATUS_OFF_MSG "0"
#define DEFAULT_PORT 1883
#define DEFAULT_KEEP_ALIVE 15
#define DEFAULT_TOPIC_PREFIX "esp/" + _client_ID
#define DEFAULT_WILL_TOPIC DEFAULT_TOPIC_PREFIX + "/status"
#define DEFAULT_COMMANDS_TOPIC DEFAULT_TOPIC_PREFIX + "/commands"


using PacketID = uint16_t;
class BasicMqtt {
  public:
	typedef std::vector<std::string> Command;
	typedef std::function<void(bool sessionPresent)> OnConnect;
	typedef std::function<void(const char* _topic, const char* _payload)> OnMessage;
	typedef std::function<void(PacketID packetId)> OnPublish;
	typedef std::function<void(AsyncMqttClientDisconnectReason reason)> OnDisconnect;
	typedef std::function<bool(Command mqttCommand)> OnCommand;
	struct Config {
		std::string broker_address;
		uint16_t broker_port;
		std::string client_ID;
		bool cleanSession;
		uint16_t keepalive;
		std::string user;
		std::string pass;
		std::string topicPrefix;
		std::string command_topic;
		std::string will_topic;
		std::string will_msg;
	};
	enum QoS {
		QoS0,
		QoS1,
		QoS2
	};
	enum ConnectionStatus {
		s_disconnected,
		s_connecting,
		s_connected
	};

	BasicMqtt(const char* broker_address);
	BasicMqtt(const char* broker_address, const char* user, const char* pass);
	BasicMqtt(const char* broker_address, bool cleanSession, const char* user, const char* pass);
	BasicMqtt(const char* broker_address, const char* clientID, const char* user, const char* pass);
	BasicMqtt(const char* broker_address, const char* clientID, bool cleanSession, const char* user, const char* pass);
	BasicMqtt(const char* broker_address, int broker_port, const char* clientID, bool cleanSession,
	          int keepAlive, const char* user, const char* pass);

	std::string topicPrefix;

	void setConfig(Config config);
	void getConfig(Config& config);
	Config getConfig();
	void addLogger(void (*logger)(String logLevel, String msg));
	void setup();
	void setWaitingFunction(void (*connectingIndicator)(u_long onTime, u_long offTime));
	bool waitForConnection(int waitTime = 10);
	void onConnect(const OnConnect& handler);
	void onMessage(const OnMessage& handler);
	void onPublish(const OnPublish& handler);
	void onDisconnect(const OnDisconnect& handler);
	void commands(const OnCommand& handler);
	// clang-format off
	PacketID publish(const char* topic, const char* payload, uint8_t qos = QoS0, bool retain = false);
	PacketID publish(const char* topic, std::string payload, uint8_t qos = QoS0, bool retain = false);
	PacketID publish(const char* topic, String payload, uint8_t qos = QoS0, bool retain = false);
	PacketID publish(const char* topic, int8_t payload, uint8_t qos = QoS0, bool retain = false) { return publish(topic, (int32_t)payload, qos, retain); };
	PacketID publish(const char* topic, int16_t payload, uint8_t qos = QoS0, bool retain = false) { return publish(topic, (int32_t)payload, qos, retain); };
	PacketID publish(const char* topic, int32_t payload, uint8_t qos = QoS0, bool retain = false);    // int
	PacketID publish(const char* topic, long payload, uint8_t qos = QoS0, bool retain = false);
	PacketID publish(const char* topic, int64_t payload, uint8_t qos = QoS0, bool retain = false);
	PacketID publish(const char* topic, uint8_t payload, uint8_t qos = QoS0, bool retain = false) { return publish(topic, (uint32_t)payload, qos, retain); };
	PacketID publish(const char* topic, uint16_t payload, uint8_t qos = QoS0, bool retain = false) { return publish(topic, (uint32_t)payload, qos, retain); };
	PacketID publish(const char* topic, uint32_t payload, uint8_t qos = QoS0, bool retain = false);
	PacketID publish(const char* topic, u_long payload, uint8_t qos = QoS0, bool retain = false);
	PacketID publish(const char* topic, uint64_t payload, uint8_t qos = QoS0, bool retain = false);
	PacketID publish(const char* topic, float payload, uint8_t qos = QoS0, bool retain = false) { return publish(topic, payload, 3, 2, qos, retain); };
	PacketID publish(const char* topic, float payload, signed char width, unsigned char prec, uint8_t qos = QoS0, bool retain = false);
	PacketID subscribe(const char* topic, uint8_t qos = QoS0);
	void setKeepAlive(uint16_t keepAlive) { _keepalive = keepAlive; };
	// clang-format on
	static void connect();
	static void reconnect();
	static void disconnect();
	bool connected();

  private:
	static std::string _broker_address;
	static uint16_t _broker_port;
	static std::string _client_ID;
	static bool _cleanSession;
	uint16_t _keepalive;
	std::string _will_topic;
	std::string _will_msg;
	static std::string _user;
	static std::string _pass;
	static bool _shouldBeConnected;
	static uint8_t _connectionStatus;
	std::string _command_topic;
	void (*_connectingIndicator)(u_long onTime, u_long offTime);
	void (*_logger)(String logLevel, String msg);
	std::vector<OnConnect> _onConnectHandlers;
	std::vector<OnMessage> _onMessageHandlers;
	std::vector<OnPublish> _onPublishHandlers;
	std::vector<OnDisconnect> _onDisconnectHandlers;
	std::vector<OnCommand> _mqttCommandsHandlers;
	void _onConnect(bool sessionPresent);
	void _onMessage(const char* _topic, const char* _payload);
	void _onPublish(PacketID packetId);
	void _onDisconnect(AsyncMqttClientDisconnectReason reason);
	bool _mqttCommands(const char* command);
	std::string _generateClientID();
};
