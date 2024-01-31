#include <esp-basic-mqtt.h>
#include <esp-basic-wifi.h>

#define WIFI_SSID "your-wifi-ssid"
#define WIFI_PASS "your-wifi-password"

#define MQTT_BROKER "mqtt-broker.lan"
#define MQTT_USER "user"
#define MQTT_PASS "password"

BasicMqtt mqtt(MQTT_BROKER, MQTT_USER, MQTT_PASS);
BasicWiFi wifi(WIFI_SSID, WIFI_PASS);

void setup() {
	Serial.begin(115200);
	Serial.println();
	mqtt.onConnect(handleMqttConnect);
	mqtt.onError(handleMqttError);
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
	h4.loop();
	delay(10);
}

void handleWiFiGotIP(GOT_IP_HANDLER_ARGS) {
	mqtt.connect();
}
void handleWiFiDisconnected(DISCONNECTED_HANDLER_ARGS) {
	mqtt.disconnect();
}

void handleMqttConnect() {
	Serial.println("User handler for MQTT onConnect");
}
void handleMqttError(int error, int info) {
	Serial.println("User handler for MQTT onError");
}
void handleMqttPublish(PacketID packetId) {
	Serial.printf("Packet: %i successfully published\n", packetId);
}
void handleMqttDisconnect() {
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
