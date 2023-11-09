#include "esp-basic-mqtt.hpp"

PangolinMQTT _clientMqtt;
Ticker _mqttReconnectTimer;

bool BasicMqtt::_shouldBeConnected = true;
std::string BasicMqtt::_client_ID = "";
bool BasicMqtt::_cleanSession = true;

BasicMqtt::BasicMqtt(const char* broker_address)
    : _connected(false)
    , _broker_address(broker_address)
    , _broker_port(DEFAULT_PORT)
    , _user("")
    , _pass("")
    , _will_msg(STATUS_OFF_MSG)
    , _keepalive(DEFAULT_KEEP_ALIVE)
    , _logger(nullptr) {
	_client_ID = _generateClientID();
	_cleanSession = true;
	_will_topic = (std::string)DEFAULT_WILL_TOPIC;
	_command_topic = (std::string)DEFAULT_COMMANDS_TOPIC;
	topicPrefix = (std::string)DEFAULT_TOPIC_PREFIX;
}
BasicMqtt::BasicMqtt(const char* broker_address, const char* user, const char* pass)
    : _connected(false)
    , _broker_address(broker_address)
    , _broker_port(DEFAULT_PORT)
    , _will_msg(STATUS_OFF_MSG)
    , _user(user)
    , _pass(pass)
    , _keepalive(DEFAULT_KEEP_ALIVE)
    , _logger(nullptr) {
	_client_ID = _generateClientID();
	_cleanSession = true;
	_will_topic = (std::string)DEFAULT_WILL_TOPIC;
	_command_topic = (std::string)DEFAULT_COMMANDS_TOPIC;
	topicPrefix = (std::string)DEFAULT_TOPIC_PREFIX;
}
BasicMqtt::BasicMqtt(const char* broker_address, bool cleanSession, const char* user, const char* pass)
    : _connected(false)
    , _broker_address(broker_address)
    , _broker_port(DEFAULT_PORT)
    , _will_msg(STATUS_OFF_MSG)
    , _user(user)
    , _pass(pass)
    , _keepalive(DEFAULT_KEEP_ALIVE)
    , _logger(nullptr) {
	_client_ID = _generateClientID();
	_cleanSession = true;
	_will_topic = (std::string)DEFAULT_WILL_TOPIC;
	_command_topic = (std::string)DEFAULT_COMMANDS_TOPIC;
	topicPrefix = (std::string)DEFAULT_TOPIC_PREFIX;
}
BasicMqtt::BasicMqtt(const char* broker_address, const char* clientID, const char* user, const char* pass)
    : _connected(false)
    , _broker_address(broker_address)
    , _broker_port(DEFAULT_PORT)
    , _will_msg(STATUS_OFF_MSG)
    , _user(user)
    , _pass(pass)
    , _keepalive(DEFAULT_KEEP_ALIVE)
    , _logger(nullptr) {
	_client_ID = clientID;
	_cleanSession = true;
	_will_topic = (std::string)DEFAULT_WILL_TOPIC;
	_command_topic = (std::string)DEFAULT_COMMANDS_TOPIC;
	topicPrefix = (std::string)DEFAULT_TOPIC_PREFIX;
}
BasicMqtt::BasicMqtt(const char* broker_address, const char* clientID, bool cleanSession, const char* user, const char* pass)
    : _connected(false)
    , _broker_address(broker_address)
    , _broker_port(DEFAULT_PORT)
    , _will_msg(STATUS_OFF_MSG)
    , _user(user)
    , _pass(pass)
    , _keepalive(DEFAULT_KEEP_ALIVE)
    , _logger(nullptr) {
	_client_ID = clientID;
	_cleanSession = cleanSession;
	_will_topic = (std::string)DEFAULT_WILL_TOPIC;
	_command_topic = (std::string)DEFAULT_COMMANDS_TOPIC;
	topicPrefix = (std::string)DEFAULT_TOPIC_PREFIX;
}
BasicMqtt::BasicMqtt(const char* broker_address, int broker_port, const char* clientID, bool cleanSession,
                     int keepAlive, const char* willTopic, const char* willMsg, const char* topicPrefix,
                     const char* commandsTopic, const char* user, const char* pass)
    : _connected(false)
    , _broker_address(broker_address)
    , _broker_port(broker_port)
    , _will_topic(willTopic)
    , _will_msg(willMsg)
    , _user(user)
    , _pass(pass)
    , _keepalive(keepAlive)
    , _command_topic(commandsTopic)
    , topicPrefix(topicPrefix)
    , _logger(nullptr) {
	_client_ID = clientID;
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
	_clientMqtt.connect(_client_ID, _cleanSession);
}
void BasicMqtt::reconnect() {
	disconnect();
	_mqttReconnectTimer.once(MQTT_RECONNECT_DELAY, []() {
		connect();
	});
}
void BasicMqtt::disconnect() {
	BasicMqtt::_shouldBeConnected = false;
	if (_clientMqtt.connected()) { _clientMqtt.disconnect(); }
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
void BasicMqtt::subscribe(const char* topic, uint8_t qos) {
	return _clientMqtt.subscribe(topic, qos);
}
bool BasicMqtt::connected() {
	return _connected;
}
void BasicMqtt::addLogger(void (*logger)(String logLevel, String msg)) {
	_logger = logger;
}
void BasicMqtt::setup() {
	_clientMqtt.setKeepAlive(_keepalive);
	_clientMqtt.setWill(_will_topic, QoS2, true, _will_msg);
	_clientMqtt.setServer((_broker_address + ":" + String(_broker_port).c_str()).c_str(), _user.c_str(), _pass.c_str(), nullptr);
	_clientMqtt.onMqttConnect([&](bool session) {
		_onConnect();
	});
	_clientMqtt.onMqttMessage([&](const char* topic, const uint8_t* payload, size_t len, uint8_t qos, bool retain, bool dup) {
		char* buf;
		_clientMqtt.xPayload(payload, len, buf);
		_onMessage(topic, buf);
		free(buf);    //* DO NOT FORGET TO DO THIS!
	});
	_clientMqtt.onMqttError([&](uint8_t e, uint32_t info) {
		_onError(e, info);
	});
	_clientMqtt.onMqttDisconnect([&](int8_t reason) {
		_onDisconnect(reason);
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
void BasicMqtt::_onError(uint8_t error, uint32_t info) {
	if (_logger != nullptr) {
		(*_logger)(
		    "mqtt",
		    (String) "MQTT error [" + String(_MQTTerror[error < 0 ? 24 : error])
		        + (error < 0 ? "(" + String(error, 10) + ")]" : "]")
		        + " error info = " + _clientMqtt.stateToString() + "(" + String(info, 10) + ")");
	}
	for (const auto& handler : _onErrorHandlers) handler(error, info);
}
void BasicMqtt::_onDisconnect(int8_t reason) {
	BASIC_MQTT_PRINTLN("MQTT disconnected: [" + String(reason, 10) + "]!");
	if (_logger != nullptr) {
		(*_logger)(
		    "mqtt",
		    (String) "MQTT disconnected [" + String(_MQTTerror[(reason < 0) ? 24 : reason]) + (reason < 0 ? "(" + String(reason, 10) + ")]" : "]"));
	}
	_connected = false;
	if (_clientMqtt.connected()) { _clientMqtt.close(); }    // make sure we are really disconnected
	if (_shouldBeConnected && !_mqttReconnectTimer.active()) {
		_mqttReconnectTimer.attach(_keepalive * PANGO_POLL_RATE, []() { connect(); });
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
	BASIC_MQTT_PRINTF("received command: %s\n", command, tokenizedCommand.size());
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
