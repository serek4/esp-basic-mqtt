#pragma once

#include <Arduino.h>
#include <espMqttClientAsync.h>
#include <Ticker.h>
#include <functional>
#include <stdint.h>
#include <string>
#include <vector>
#include <list>

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
#define DEFAULT_TOPIC_PREFIX "esp/"
#define DEFAULT_COMMANDS_TOPIC_SUFFIX "/commands"
#define DEFAULT_WILL_TOPIC_SUFFIX "/status"


using PacketID = uint16_t;
class BasicMqtt {
  public:
	typedef std::vector<std::string> Command;
	typedef std::function<void(bool sessionPresent)> OnConnect;
	typedef std::function<void(const char* _topic, const char* _payload)> OnMessage;
	typedef std::function<void(PacketID packetId)> OnPublish;
	typedef std::function<void(espMqttClientTypes::DisconnectReason reason)> OnDisconnect;
	typedef std::function<bool(Command mqttCommand)> OnCommand;
	struct Config {
		Config();
		uint16_t broker_port;
		uint16_t keepalive;
		std::string broker_address;
		std::string user;
		std::string pass;
		std::string clientID;
		std::string topicPrefix;
		std::string command_topic;
		std::string will_topic;
		std::string will_msg;
		bool cleanSession;
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
	BasicMqtt(const char* broker_address, int broker_port, const char* user, const char* pass);

	std::string& topicPrefix;

	void setConfig(const Config& config);
	Config getConfig();
	void addLogger(void (*logger)(String logLevel, String msg));
	void setCleanSession(bool cleanSession);
	void setKeepAlive(uint16_t keepAlive);
	void setclientID(const char* clientID);
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
	// clang-format on
	static void connect();
	static void reconnect();
	static void disconnect();
	bool connected();

  private:
	Config _config;

	static bool _shouldBeConnected;
	static uint8_t _connectionStatus;
	void (*_connectingIndicator)(u_long onTime, u_long offTime);
	void (*_logger)(String logLevel, String msg);
	std::list<OnConnect> _onConnectHandlers;
	std::list<OnMessage> _onMessageHandlers;
	std::list<OnPublish> _onPublishHandlers;
	std::list<OnDisconnect> _onDisconnectHandlers;
	std::list<OnCommand> _mqttCommandsHandlers;
	void _onConnect(bool sessionPresent);
	void _onMessage(const char* _topic, const char* _payload);
	void _onPublish(PacketID packetId);
	void _onDisconnect(espMqttClientTypes::DisconnectReason reason);
	bool _mqttCommands(const char* command);
	std::string _generateClientID();
	std::string _generateTopicPrefix(const char* prefix = DEFAULT_TOPIC_PREFIX);
	std::string _generateCommandTopic(const char* suffix = DEFAULT_COMMANDS_TOPIC_SUFFIX);
	std::string _generateWillTopic(const char* suffix = DEFAULT_WILL_TOPIC_SUFFIX);
};
