#include "esp-basic-mqtt.hpp"

H4AsyncMQTT _clientMqtt;
H4 h4;
Ticker _mqttReconnectTimer;

std::string BasicMqtt::_broker_address = "";
uint16_t BasicMqtt::_broker_port = DEFAULT_PORT;
std::string BasicMqtt::_user = "";
std::string BasicMqtt::_pass = "";
bool BasicMqtt::_shouldBeConnected = true;
bool BasicMqtt::_connected = false;
std::string BasicMqtt::_client_ID = "";
bool BasicMqtt::_cleanSession = true;

BasicMqtt::BasicMqtt(const char* broker_address)
    : _will_msg(STATUS_OFF_MSG)
    , _keepalive(DEFAULT_KEEP_ALIVE)
    , _logger(nullptr) {
	_broker_address = broker_address;
	_client_ID = _generateClientID();
	_cleanSession = true;
	_will_topic = (std::string)DEFAULT_WILL_TOPIC;
	_command_topic = (std::string)DEFAULT_COMMANDS_TOPIC;
	topicPrefix = (std::string)DEFAULT_TOPIC_PREFIX;
}
BasicMqtt::BasicMqtt(const char* broker_address, const char* user, const char* pass)
    : _will_msg(STATUS_OFF_MSG)
    , _keepalive(DEFAULT_KEEP_ALIVE)
    , _logger(nullptr) {
	_broker_address = broker_address;
	_client_ID = _generateClientID();
	_cleanSession = true;
	_user = user;
	_pass = pass;
	_will_topic = (std::string)DEFAULT_WILL_TOPIC;
	_command_topic = (std::string)DEFAULT_COMMANDS_TOPIC;
	topicPrefix = (std::string)DEFAULT_TOPIC_PREFIX;
}
BasicMqtt::BasicMqtt(const char* broker_address, bool cleanSession, const char* user, const char* pass)
    : _will_msg(STATUS_OFF_MSG)
    , _keepalive(DEFAULT_KEEP_ALIVE)
    , _logger(nullptr) {
	_broker_address = broker_address;
	_client_ID = _generateClientID();
	_cleanSession = true;
	_user = user;
	_pass = pass;
	_will_topic = (std::string)DEFAULT_WILL_TOPIC;
	_command_topic = (std::string)DEFAULT_COMMANDS_TOPIC;
	topicPrefix = (std::string)DEFAULT_TOPIC_PREFIX;
}
BasicMqtt::BasicMqtt(const char* broker_address, const char* clientID, const char* user, const char* pass)
    : _will_msg(STATUS_OFF_MSG)
    , _keepalive(DEFAULT_KEEP_ALIVE)
    , _logger(nullptr) {
	_broker_address = broker_address;
	_client_ID = clientID;
	_cleanSession = true;
	_user = user;
	_pass = pass;
	_will_topic = (std::string)DEFAULT_WILL_TOPIC;
	_command_topic = (std::string)DEFAULT_COMMANDS_TOPIC;
	topicPrefix = (std::string)DEFAULT_TOPIC_PREFIX;
}
BasicMqtt::BasicMqtt(const char* broker_address, const char* clientID, bool cleanSession, const char* user, const char* pass)
    : _will_msg(STATUS_OFF_MSG)
    , _keepalive(DEFAULT_KEEP_ALIVE)
    , _logger(nullptr) {
	_broker_address = broker_address;
	_client_ID = clientID;
	_cleanSession = cleanSession;
	_user = user;
	_pass = pass;
	_will_topic = (std::string)DEFAULT_WILL_TOPIC;
	_command_topic = (std::string)DEFAULT_COMMANDS_TOPIC;
	topicPrefix = (std::string)DEFAULT_TOPIC_PREFIX;
}
BasicMqtt::BasicMqtt(const char* broker_address, int broker_port, const char* clientID, bool cleanSession,
                     int keepAlive, const char* willTopic, const char* willMsg, const char* topicPrefix,
                     const char* commandsTopic, const char* user, const char* pass)
    : _will_topic(willTopic)
    , _will_msg(willMsg)
    , _keepalive(keepAlive)
    , _command_topic(commandsTopic)
    , topicPrefix(topicPrefix)
    , _logger(nullptr) {
	_broker_address = broker_address;
	_broker_port = broker_port;
	_client_ID = clientID;
	_user = user;
	_pass = pass;
	_cleanSession = cleanSession;
}

void BasicMqtt::setConfig(Config config) {
	_broker_address = config.broker_address;
	_broker_port = config.broker_port;
	_client_ID = config.client_ID;
	_cleanSession = config.cleanSession;
	_keepalive = config.keepalive;
	_user = config.user;
	_pass = config.pass;
	topicPrefix = config.topicPrefix;
	_command_topic = config.command_topic;
	_will_topic = config.will_topic;
	_will_msg = config.will_msg;
}
void BasicMqtt::getConfig(Config& config) {
	config.broker_address = _broker_address;
	config.broker_port = _broker_port;
	config.client_ID = _client_ID;
	config.cleanSession = _cleanSession;
	config.keepalive = _keepalive;
	config.user = _user;
	config.pass = _pass;
	config.topicPrefix = topicPrefix;
	config.command_topic = _command_topic;
	config.will_topic = _will_topic;
	config.will_msg = _will_msg;
}
BasicMqtt::Config BasicMqtt::getConfig() {
	Config config;
	config.broker_address = _broker_address;
	config.broker_port = _broker_port;
	config.client_ID = _client_ID;
	config.cleanSession = _cleanSession;
	config.keepalive = _keepalive;
	config.user = _user;
	config.pass = _pass;
	config.topicPrefix = topicPrefix;
	config.command_topic = _command_topic;
	config.will_topic = _will_topic;
	config.will_msg = _will_msg;
	return config;
}
void BasicMqtt::connect() {
	BasicMqtt::_shouldBeConnected = true;
	if (!_connected) {
		_clientMqtt.connect((_broker_address + ":" + String(_broker_port).c_str()).c_str(), _user.c_str(), _pass.c_str(), _client_ID.c_str());
	}
}
void BasicMqtt::reconnect() {
	disconnect();
	_mqttReconnectTimer.once(MQTT_MANUAL_RECONNECT_DELAY, []() {
		connect();
	});
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
void BasicMqtt::onError(const OnError& handler) {
	_onErrorHandlers.push_back(handler);
}
void BasicMqtt::onDisconnect(const OnDisconnect& handler) {
	_onDisconnectHandlers.push_back(handler);
}
void BasicMqtt::commands(const OnCommand& handler) {
	_mqttCommandsHandlers.push_back(handler);
}
void BasicMqtt::publish(const char* topic, const char* payload, uint8_t qos, bool retain) {
	if (_connected) {
		_clientMqtt.publish(topic, payload, strlen(payload), qos, retain);
	}
}
void BasicMqtt::publish(const char* topic, std::string payload, uint8_t qos, bool retain) {
	if (_connected) {
		_clientMqtt.publish(topic, payload.c_str(), payload.length(), qos, retain);
	}
}
void BasicMqtt::publish(const char* topic, String payload, uint8_t qos, bool retain) {
	if (_connected) {
		_clientMqtt.publish(topic, payload.c_str(), payload.length(), qos, retain);
	}
}
void BasicMqtt::publish(const char* topic, int32_t payload, uint8_t qos, bool retain) {
	if (_connected) {
		char numberBuffer[12];
		itoa(payload, numberBuffer, 10);
		_clientMqtt.publish(topic, (uint8_t*)numberBuffer, (size_t)strlen(numberBuffer), qos, retain);
	}
}
void BasicMqtt::publish(const char* topic, long payload, uint8_t qos, bool retain) {
	if (_connected) {
		char numberBuffer[12];
		ltoa(payload, numberBuffer, 10);
		_clientMqtt.publish(topic, (uint8_t*)numberBuffer, (size_t)strlen(numberBuffer), qos, retain);
	}
}
void BasicMqtt::publish(const char* topic, uint32_t payload, uint8_t qos, bool retain) {
	if (_connected) {
		char numberBuffer[12];
		utoa(payload, numberBuffer, 10);
		_clientMqtt.publish(topic, (uint8_t*)numberBuffer, (size_t)strlen(numberBuffer), qos, retain);
	}
}
void BasicMqtt::publish(const char* topic, u_long payload, uint8_t qos, bool retain) {
	if (_connected) {
		char numberBuffer[12];
		ultoa(payload, numberBuffer, 10);
		_clientMqtt.publish(topic, (uint8_t*)numberBuffer, (size_t)strlen(numberBuffer), qos, retain);
	}
}
void BasicMqtt::publish(const char* topic, int64_t payload, uint8_t qos, bool retain) {
	if (_connected) {
		char numberBuffer[21];
#ifdef ARDUINO_ARCH_ESP32
		lltoa(payload, numberBuffer, 10);
#elif defined(ARDUINO_ARCH_ESP8266)
		lltoa(payload, numberBuffer, sizeof(numberBuffer), 10);
#endif
		_clientMqtt.publish(topic, (uint8_t*)numberBuffer, (size_t)strlen(numberBuffer), qos, retain);
	}
}
void BasicMqtt::publish(const char* topic, uint64_t payload, uint8_t qos, bool retain) {
	if (_connected) {
		char numberBuffer[21];
#ifdef ARDUINO_ARCH_ESP32
		ulltoa(payload, numberBuffer, 10);
#elif defined(ARDUINO_ARCH_ESP8266)
		ulltoa(payload, numberBuffer, sizeof(numberBuffer), 10);
#endif
		_clientMqtt.publish(topic, (uint8_t*)numberBuffer, (size_t)strlen(numberBuffer), qos, retain);
	}
}
void BasicMqtt::publish(const char* topic, float payload, signed char width, unsigned char prec, uint8_t qos, bool retain) {
	if (_connected) {
		char numberBuffer[21];
		dtostrf(payload, width, prec, numberBuffer);
		_clientMqtt.publish(topic, (uint8_t*)numberBuffer, (size_t)strlen(numberBuffer), qos, retain);
	}
}
uint32_t BasicMqtt::subscribe(const char* topic, uint8_t qos) {
	return _clientMqtt.subscribe(topic, qos);
}
bool BasicMqtt::connected() {
	return _connected;
}
void BasicMqtt::addLogger(void (*logger)(String logLevel, String msg)) {
	_logger = logger;
}
void BasicMqtt::setup() {
	h4.setup();
	_clientMqtt.setKeepAlive(_keepalive);
	_clientMqtt.setWill(_will_topic.c_str(), QoS2, _will_msg.c_str(), true);
	_clientMqtt.onConnect([&](H4AMC_ConnackParam params) {
		_onConnect();
	});
	_clientMqtt.onMessage([&](const char* topic, const uint8_t* payload, size_t len, H4AMC_MessageOptions opts) {
		char* buf;
		_clientMqtt.xPayload(payload, len, buf);
		_onMessage(topic, buf);
		free(buf);    //* DO NOT FORGET TO DO THIS!
	});
	_clientMqtt.onError([&](int e, int info) {
		_onError(e, info);
	});
	_clientMqtt.onDisconnect([&]() {
		_onDisconnect();
	});
}
void BasicMqtt::setWaitingFunction(void (*connectingIndicator)(u_long onTime, u_long offTime)) {
	_connectingIndicator = connectingIndicator;
}
bool BasicMqtt::waitForConnection(int waitTime) {
	u_long startWaitingAt = millis();
	if (!_connected) {
		BASIC_MQTT_PRINT("Connecting MQTT");
		while (!_connected) {
			h4.loop();    // let it do it's stuff while waiting (H4AsyncMQTT dependency)
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

void BasicMqtt::_onConnect() {
	BASIC_MQTT_PRINTLN((String) "MQTT connected!\n " + _client_ID.c_str() + "@" + _broker_address.c_str());
	if (_logger != nullptr) {
		(*_logger)("mqtt", (String) "MQTT connected [" + _client_ID.c_str() + "@" + _broker_address.c_str() + "]");
	}
	_connected = true;
	_mqttReconnectTimer.detach();
	_clientMqtt.publish((topicPrefix + "/status").c_str(), STATUS_ON_MSG, strlen(STATUS_ON_MSG), QoS0, true);
	_clientMqtt.subscribe(_command_topic.c_str(), QoS0);
	_clientMqtt.subscribe((topicPrefix + "/status").c_str(), QoS0);
	for (const auto& handler : _onConnectHandlers) handler();
}
void BasicMqtt::_onMessage(const char* _topic, const char* _payload) {
	BASIC_MQTT_PRINTF("received message!\n msg.topic:   %s\n msg.payload: %s\n", _topic, _payload);
	if (_topic == _will_topic) {
		if (_payload == _will_msg) {
			_clientMqtt.publish((topicPrefix + "/status").c_str(), STATUS_ON_MSG, strlen(STATUS_ON_MSG), QoS0, true);
		}
	}
	if (_topic == _command_topic) {
		_mqttCommands(_payload);
		return;
	}
	for (const auto& handler : _onMessageHandlers) handler(_topic, _payload);
}
void BasicMqtt::_onError(int error, int info) {
	if (error == ERR_OK) { return; }
	String errorString = "";
	if (error < H4AMC_ERROR_BASE) {    // H4ASYNC error
		errorString = "H4AsyncTCP:" + (String)H4AsyncClient::errorstring(error).data() + "(" + String(error) + ")";
	} else {
		if (error < H4AMC_ERROR_MAX) {    // H4AsyncMQTT error
			errorString = "H4AsyncMQTT:" + (String)H4AsyncMQTT::errorstring(error).data() + "(" + String(error) + ")";
		} else {    // UNKNOWN error
			errorString = String(error);
		}
	}
	BASIC_MQTT_PRINTLN(errorString);
	if (_logger != nullptr) {
		(*_logger)("mqtt", (String) "MQTT error [" + errorString + "]" + " extra info: " + String(info));
	}
	for (const auto& handler : _onErrorHandlers) handler(error, info);
}
void BasicMqtt::_onDisconnect() {
	BASIC_MQTT_PRINTLN("MQTT disconnected");
	if (_logger != nullptr) { (*_logger)("mqtt", (String) "MQTT disconnected"); }
	_connected = false;
	if (_shouldBeConnected && !_mqttReconnectTimer.active()) {
		_mqttReconnectTimer.attach(MQTT_AUTO_RECONNECT_DELAY, []() { _clientMqtt.reconnect(); });
	}
	for (const auto& handler : _onDisconnectHandlers) handler();
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
