#include <esp-basic-mqtt.h>
#include <esp-basic-wifi.h>

#define WIFI_SSID "your-wifi-ssid"
#define WIFI_PASS "your-wifi-password"

#define MQTT_BROKER "mqtt-broker.lan"
#define MQTT_USER "user"
#define MQTT_PASS "password"

BasicMqtt mqtt(MQTT_BROKER, MQTT_USER, MQTT_PASS);
BasicWiFi wifi(WIFI_SSID, WIFI_PASS);

long loopDelay = -50000;
void setup() {
	Serial.begin(115200);
	Serial.println();
	mqtt.onConnect(handleMqttConnect);
	mqtt.onPublish(handleMqttPublish);
	mqtt.onDisconnect(handleMqttDisconnect);
	mqtt.onMessage(handleIncMqttMsg);
	mqtt.commands(handleMqttCommands);
	mqtt.setup();
	wifi.onGotIP(handleWiFiGotIP);
	wifi.onDisconnected(handleWiFiDisconnected);
	wifi.setup();
	if (wifi.waitForConnection() >= BasicWiFi::wifi_got_ip) {
		mqtt.waitForConnection();
	}
	Serial.println("setup done!");
}

void loop() {
	if (millis() - loopDelay >= 60000) {
		loopDelay = millis();
		mqtt.publish((mqtt.topicPrefix + "/loop").c_str(), millis(), BasicMqtt::QoS1);

		int intTest = INT32_MIN;
		long longTest = INT32_MIN;
		long long longLongTest = INT64_MIN;
		int8_t int8Test = INT8_MIN;
		int16_t int16Test = INT16_MIN;
		int32_t int32Test = INT32_MIN;
		int64_t int64Test = INT64_MIN;

		unsigned int unsignedIntTest = UINT32_MAX;
		unsigned long unsignedLongTest = UINT32_MAX;
		unsigned long long unsignedLonglongTest = UINT64_MAX;
		uint8_t uint8Test = UINT8_MAX;
		uint16_t uint16Test = UINT16_MAX;
		uint32_t uint32Test = UINT32_MAX;
		uint64_t uint64Test = UINT64_MAX;
		u_long ulongTest = UINT32_MAX;

		float floatTest = 2.0 / 3.0;


		mqtt.publish((mqtt.topicPrefix + "/test").c_str(), intTest);
		mqtt.publish((mqtt.topicPrefix + "/test").c_str(), longTest);
		mqtt.publish((mqtt.topicPrefix + "/test").c_str(), longLongTest);
		mqtt.publish((mqtt.topicPrefix + "/test").c_str(), int8Test);
		mqtt.publish((mqtt.topicPrefix + "/test").c_str(), int16Test);
		mqtt.publish((mqtt.topicPrefix + "/test").c_str(), int32Test);
		mqtt.publish((mqtt.topicPrefix + "/test").c_str(), int64Test);

		mqtt.publish((mqtt.topicPrefix + "/test").c_str(), unsignedIntTest);
		mqtt.publish((mqtt.topicPrefix + "/test").c_str(), unsignedLongTest);
		mqtt.publish((mqtt.topicPrefix + "/test").c_str(), unsignedLonglongTest);
		mqtt.publish((mqtt.topicPrefix + "/test").c_str(), uint8Test);
		mqtt.publish((mqtt.topicPrefix + "/test").c_str(), uint16Test);
		mqtt.publish((mqtt.topicPrefix + "/test").c_str(), uint32Test);
		mqtt.publish((mqtt.topicPrefix + "/test").c_str(), uint64Test);
		mqtt.publish((mqtt.topicPrefix + "/test").c_str(), ulongTest);

		mqtt.publish((mqtt.topicPrefix + "/test").c_str(), floatTest);
	}
}

void handleWiFiGotIP(GOT_IP_HANDLER_ARGS) {
	mqtt.connect();
}
void handleWiFiDisconnected(DISCONNECTED_HANDLER_ARGS) {
	mqtt.disconnect();
}

void handleMqttConnect(bool sessionPresent) {
	Serial.println("User handler for MQTT onConnect");
}
void handleMqttPublish(PacketID packetId) {
	Serial.printf("Packet: %i successfully published\n", packetId);
}
void handleMqttDisconnect(espMqttClientTypes::DisconnectReason reason) {
	Serial.println("User handler for MQTT onDisconnect");
}
void handleIncMqttMsg(const char* topic, const char* payload) {
	Serial.printf("Incoming mqtt message!\n msg.topic:   %s\n msg.payload: %s\n", topic, payload);
	mqtt.publish((mqtt.topicPrefix + "/feedback").c_str(), payload);
}
bool handleMqttCommands(BasicMqtt::Command mqttCommand) {
	if (mqttCommand[0] == "user") {
		if (mqttCommand.size() > 1) {
			if (mqttCommand[1] == "command") {
				Serial.println("user command executed");
				return true;
			}
		}
	}
	return false;
}
