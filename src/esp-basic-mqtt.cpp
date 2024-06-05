#include "esp-basic-mqtt.hpp"

espMqttClientAsync _clientMqtt;
Ticker _mqttReconnectTimer;
bool BasicMqtt::_shouldBeConnected = true;
uint8_t BasicMqtt::_connectionStatus = s_disconnected;

BasicMqtt::Config::Config()
    : broker_port(DEFAULT_PORT)
    , keepalive(DEFAULT_KEEP_ALIVE)
    , broker_address("")
    , user("")
    , pass("")
    , clientID("")
    , topicPrefix("")
    , command_topic("")
    , will_topic("")
    , will_msg(STATUS_OFF_MSG)
    , cleanSession(true) {
}

BasicMqtt::BasicMqtt(const char* broker_address)
    : BasicMqtt::BasicMqtt(broker_address, DEFAULT_PORT, "", "") {
}
BasicMqtt::BasicMqtt(const char* broker_address, const char* user, const char* pass)
    : BasicMqtt::BasicMqtt(broker_address, DEFAULT_PORT, user, pass) {
}
BasicMqtt::BasicMqtt(const char* broker_address, int broker_port, const char* user, const char* pass)
    : topicPrefix(_config.topicPrefix)
    , _logger(nullptr) {
	_config.broker_port = broker_port;
	_config.broker_address = broker_address;
	_config.user = user;
	_config.pass = pass;
	_config.clientID = _generateClientID();
	_config.topicPrefix = _generateTopicPrefix();
	_config.command_topic = _generateCommandTopic();
	_config.will_topic = _generateWillTopic();
}

void BasicMqtt::setConfig(const Config& config) {
	_config.broker_address = config.broker_address;
	_config.broker_port = config.broker_port;
	_config.clientID = config.clientID;
	_config.cleanSession = config.cleanSession;
	_config.keepalive = config.keepalive;
	_config.user = config.user;
	_config.pass = config.pass;
	_config.topicPrefix = config.topicPrefix;
	_config.command_topic = config.command_topic;
	_config.will_topic = config.will_topic;
	_config.will_msg = config.will_msg;
}
BasicMqtt::Config BasicMqtt::getConfig() {
	return _config;
}
void BasicMqtt::connect() {
	BasicMqtt::_shouldBeConnected = true;
	if (_connectionStatus == s_disconnected) {
		_clientMqtt.connect();
		_connectionStatus = s_connecting;
	}
}
void BasicMqtt::reconnect() {
	if (_connectionStatus != s_disconnected) { disconnect(); }
	if (!_mqttReconnectTimer.active()) {
		_mqttReconnectTimer.once(MQTT_MANUAL_RECONNECT_DELAY, []() {
			connect();
		});
	}
}
void BasicMqtt::disconnect() {
	BasicMqtt::_shouldBeConnected = false;
	_clientMqtt.disconnect();
	_mqttReconnectTimer.detach();
}
void BasicMqtt::onConnect(const OnConnect& handler) {
	_onConnectHandlers.push_back(handler);
}
void BasicMqtt::onMessage(const OnMessage& handler) {
	_onMessageHandlers.push_back(handler);
}
void BasicMqtt::onPublish(const OnPublish& handler) {
	_onPublishHandlers.push_back(handler);
}
void BasicMqtt::onDisconnect(const OnDisconnect& handler) {
	_onDisconnectHandlers.push_back(handler);
}
void BasicMqtt::commands(const OnCommand& handler) {
	_mqttCommandsHandlers.push_back(handler);
}
PacketID BasicMqtt::publish(const char* topic, const char* payload, uint8_t qos, bool retain) {
	if (_connectionStatus == s_connected) {
		return _clientMqtt.publish(topic, qos, retain, payload);
	}
	return (PacketID)0;
}
PacketID BasicMqtt::publish(const char* topic, std::string payload, uint8_t qos, bool retain) {
	if (_connectionStatus == s_connected) {
		return _clientMqtt.publish(topic, qos, retain, payload.c_str());
	}
	return (PacketID)0;
}
PacketID BasicMqtt::publish(const char* topic, String payload, uint8_t qos, bool retain) {
	if (_connectionStatus == s_connected) {
		return _clientMqtt.publish(topic, qos, retain, payload.c_str());
	}
	return (PacketID)0;
}
PacketID BasicMqtt::publish(const char* topic, int32_t payload, uint8_t qos, bool retain) {
	if (_connectionStatus == s_connected) {
		char numberBuffer[12];
		itoa(payload, numberBuffer, 10);
		return _clientMqtt.publish(topic, qos, retain, numberBuffer);
	}
	return (PacketID)0;
}
PacketID BasicMqtt::publish(const char* topic, long payload, uint8_t qos, bool retain) {
	if (_connectionStatus == s_connected) {
		char numberBuffer[12];
		ltoa(payload, numberBuffer, 10);
		return _clientMqtt.publish(topic, qos, retain, numberBuffer);
	}
	return (PacketID)0;
}
PacketID BasicMqtt::publish(const char* topic, uint32_t payload, uint8_t qos, bool retain) {
	if (_connectionStatus == s_connected) {
		char numberBuffer[12];
		utoa(payload, numberBuffer, 10);
		return _clientMqtt.publish(topic, qos, retain, numberBuffer);
	}
	return (PacketID)0;
}
PacketID BasicMqtt::publish(const char* topic, u_long payload, uint8_t qos, bool retain) {
	if (_connectionStatus == s_connected) {
		char numberBuffer[12];
		ultoa(payload, numberBuffer, 10);
		return _clientMqtt.publish(topic, qos, retain, numberBuffer);
	}
	return (PacketID)0;
}
PacketID BasicMqtt::publish(const char* topic, int64_t payload, uint8_t qos, bool retain) {
	if (_connectionStatus == s_connected) {
		char numberBuffer[21];
#ifdef ARDUINO_ARCH_ESP32
		lltoa(payload, numberBuffer, 10);
#elif defined(ARDUINO_ARCH_ESP8266)
		lltoa(payload, numberBuffer, sizeof(numberBuffer), 10);
#endif
		return _clientMqtt.publish(topic, qos, retain, numberBuffer);
	}
	return (PacketID)0;
}
PacketID BasicMqtt::publish(const char* topic, uint64_t payload, uint8_t qos, bool retain) {
	if (_connectionStatus == s_connected) {
		char numberBuffer[21];
#ifdef ARDUINO_ARCH_ESP32
		ulltoa(payload, numberBuffer, 10);
#elif defined(ARDUINO_ARCH_ESP8266)
		ulltoa(payload, numberBuffer, sizeof(numberBuffer), 10);
#endif
		return _clientMqtt.publish(topic, qos, retain, numberBuffer);
	}
	return (PacketID)0;
}
PacketID BasicMqtt::publish(const char* topic, float payload, signed char width, unsigned char prec, uint8_t qos, bool retain) {
	if (_connectionStatus == s_connected) {
		char numberBuffer[21];
		dtostrf(payload, width, prec, numberBuffer);
		return _clientMqtt.publish(topic, qos, retain, numberBuffer);
	}
	return (PacketID)0;
}
PacketID BasicMqtt::subscribe(const char* topic, uint8_t qos) {
	return _clientMqtt.subscribe(topic, qos);
}
bool BasicMqtt::connected() {
	return _connectionStatus == s_connected;
}
void BasicMqtt::addLogger(void (*logger)(String logLevel, String msg)) {
	_logger = logger;
}
void BasicMqtt::setCleanSession(bool cleanSession) {
	_config.cleanSession = cleanSession;
}
void BasicMqtt::setKeepAlive(uint16_t keepAlive) {
	_config.keepalive = keepAlive;
}
void BasicMqtt::setclientID(const char* clientID) {
	_config.clientID = clientID;
	_config.topicPrefix = _generateTopicPrefix();
	_config.command_topic = _generateCommandTopic();
	_config.will_topic = _generateWillTopic();
}
void BasicMqtt::setup() {
	_clientMqtt.setKeepAlive(_config.keepalive);
	_clientMqtt.setClientId(_config.clientID.c_str());
	_clientMqtt.setCleanSession(_config.cleanSession);
	_clientMqtt.setCredentials(_config.user.c_str(), _config.pass.c_str());
	_clientMqtt.setWill(_config.will_topic.c_str(), QoS2, true, _config.will_msg.c_str());
	_clientMqtt.setServer(_config.broker_address.c_str(), _config.broker_port);
	_clientMqtt.onConnect([&](bool sessionPresent) {
		_onConnect(sessionPresent);
	});
	_clientMqtt.onMessage([&](const espMqttClientTypes::MessageProperties& properties, const char* topic, const uint8_t* payload, size_t len, size_t index, size_t total) {
		if (len != total || index != 0) {
			if (_logger != nullptr) { (*_logger)("mqtt", "partial msg received at " + String(topic)); }
			// TODO handle chunked msgs
			return;
		}
		char* buffer = new char[len + 1];
		memcpy(buffer, payload, len);
		buffer[len] = '\0';
		_onMessage(topic, buffer);
		delete[] buffer;
	});
	_clientMqtt.onPublish([&](uint16_t packetId) {
		_onPublish((PacketID)packetId);
	});
	_clientMqtt.onDisconnect([&](espMqttClientTypes::DisconnectReason reason) {
		_onDisconnect(reason);
	});
}
void BasicMqtt::setWaitingFunction(void (*connectingIndicator)(u_long onTime, u_long offTime)) {
	_connectingIndicator = connectingIndicator;
}
bool BasicMqtt::waitForConnection(int waitTime) {
	u_long startWaitingAt = millis();
	if (_connectionStatus != s_connected) {
		BASIC_MQTT_PRINT("Connecting MQTT");
		while (_connectionStatus != s_connected) {
			BASIC_MQTT_PRINT(".");
			if (_connectingIndicator == nullptr) {
				delay(MQTT_NO_BLINK);
			} else {
				(*_connectingIndicator)(MQTT_BLINK_ON, MQTT_BLINK_OFF);
			}
			if (millis() - startWaitingAt > waitTime * 1000) {
				BASIC_MQTT_PRINTLN("Can't connect to MQTT!");
				return false;
				break;
			}
		}
		BASIC_MQTT_PRINTLN();
	}
	return true;
}

void BasicMqtt::_onConnect(bool sessionPresent) {
	BASIC_MQTT_PRINTLN((String) "MQTT connected!\n " + _config.clientID.c_str() + "@" + _config.broker_address.c_str());
	if (_logger != nullptr) {
		(*_logger)("mqtt", (String) "MQTT connected [" + _config.clientID.c_str() + "@" + _config.broker_address.c_str() + "]");
	}
	_connectionStatus = s_connected;
	_mqttReconnectTimer.detach();
	_clientMqtt.publish((_config.topicPrefix + "/status").c_str(), QoS0, true, STATUS_ON_MSG);
	_clientMqtt.subscribe(_config.command_topic.c_str(), QoS0);
	for (const auto& handler : _onConnectHandlers) handler(sessionPresent);
}
void BasicMqtt::_onMessage(const char* topic, const char* payload) {
	BASIC_MQTT_PRINTF("received message!\n msg.topic:   %s\n msg.payload: %s\n", topic, payload);
	if (topic == _config.command_topic) {
		_mqttCommands(payload);
		return;
	}
	for (const auto& handler : _onMessageHandlers) handler(topic, payload);
}
void BasicMqtt::_onPublish(PacketID packetId) {
	BASIC_MQTT_PRINTF("Packet: %i published\n", packetId);
	for (const auto& handler : _onPublishHandlers) handler(packetId);
}
void BasicMqtt::_onDisconnect(espMqttClientTypes::DisconnectReason reason) {
	BASIC_MQTT_PRINTF("MQTT disconnected, reason: %s\n", (String)disconnectReasonToString(reason));
	if (_logger != nullptr) { (*_logger)("mqtt", "MQTT disconnected: " + (String)disconnectReasonToString(reason)); }
	_connectionStatus = s_disconnected;
	if (_shouldBeConnected && !_mqttReconnectTimer.active()) {
		_mqttReconnectTimer.attach(MQTT_AUTO_RECONNECT_DELAY, []() {
			_clientMqtt.connect();
			_connectionStatus = s_connecting;
		});
	}
	for (const auto& handler : _onDisconnectHandlers) handler(reason);
}
bool BasicMqtt::_mqttCommands(const char* command) {
	char commandStr[strlen(command) + 1];
	strcpy(commandStr, command);
	Command tokenizedCommand;
	char* token;
	// push command tokens into vector
	token = strtok(commandStr, " ");
	while (token != NULL) {
		tokenizedCommand.push_back(token);
		token = strtok(NULL, " ");
	}
	if (tokenizedCommand.size() == 0) {
		BASIC_MQTT_PRINTF("received invalid command (%i tokens)\n", tokenizedCommand.size());
		return false;
	}
	BASIC_MQTT_PRINTF("received command: %s\n", command);
	// builtin commands
	if (tokenizedCommand[0] == "mqtt") {
		if (tokenizedCommand.size() > 1) {
			if (tokenizedCommand[1] == "reconnect") {
				BASIC_MQTT_PRINTLN("reconnecting mqtt");
				if (_logger != nullptr) { (*_logger)("mqtt", "manual mqtt reconnect"); }
				reconnect();
				return true;
			}
		}
	}
	for (const auto& handler : _mqttCommandsHandlers) {
		if (handler(tokenizedCommand)) {
			return true;
		}
	}
	BASIC_MQTT_PRINTF("command unknown: %s(%i tokens)\n", command, tokenizedCommand.size());
	return false;
}
std::string BasicMqtt::_generateClientID() {
#ifdef ARDUINO_ARCH_ESP32
	char id[13];
	sprintf(id, "%012llX", ESP.getEfuseMac());
	// read from back to front by 2 characters
	return (std::string) "esp_" + id[4] + id[5] + id[2] + id[3] + id[0] + id[1];
#elif defined(ARDUINO_ARCH_ESP8266)
	char id[7];
	sprintf(id, "%06X", ESP.getChipId());
	return (std::string) "esp_" + id;
#endif
}
std::string BasicMqtt::_generateTopicPrefix(const char* prefix) {
	return (std::string)prefix + _config.clientID;
}
std::string BasicMqtt::_generateCommandTopic(const char* suffix) {
	return (std::string)_config.topicPrefix + suffix;
}
std::string BasicMqtt::_generateWillTopic(const char* suffix) {
	return (std::string)_config.topicPrefix + suffix;
}