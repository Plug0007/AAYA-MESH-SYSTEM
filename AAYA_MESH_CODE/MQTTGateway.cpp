#include "MQTTGateway.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include "RoleController.h"
#include "NodeIdentity.h"
#include "PacketManager.h"

const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

static unsigned long lastReconnectAttempt = 0;
static unsigned long lastHeartbeat = 0;

#define MQTT_RETRY_INTERVAL 5000
#define HEARTBEAT_INTERVAL 10000

// ===== MQTT CALLBACK =====
void mqttCallback(char* topic, byte* payload, unsigned int length) {

    String message = "";

    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }

    Serial.print("MQTT Command Received: ");
    Serial.println(message);

    // Forward command to mesh
    PacketManager::sendActuatorCommand(message);
}

// ================= BEGIN =================
void MQTTGateway::begin() {

    if (RoleController::getRole() != ROLE_SINK) return;

    client.setServer(mqttServer, mqttPort);
    client.setCallback(mqttCallback);

    Serial.println("MQTT Gateway Initialized");
}

// ================= STATUS =================
bool MQTTGateway::isConnected() {

    if (RoleController::getRole() != ROLE_SINK) return false;
    if (WiFi.status() != WL_CONNECTED) return false;

    return client.connected();
}

// ================= MAINTAIN CONNECTION =================
void MQTTGateway::maintainConnection() {

    if (RoleController::getRole() != ROLE_SINK) return;
    if (WiFi.status() != WL_CONNECTED) return;

    if (client.connected()) {

        client.loop();

        if (millis() - lastHeartbeat > HEARTBEAT_INTERVAL) {

            String nodeID = NodeIdentity::getNodeID();
            String topic = "mesh/node/" + nodeID + "/heartbeat";

            client.publish(topic.c_str(), "alive", false);
            lastHeartbeat = millis();
        }

        return;
    }

    if (millis() - lastReconnectAttempt > MQTT_RETRY_INTERVAL) {

        Serial.println("Connecting to MQTT...");

        String nodeID = NodeIdentity::getNodeID();
        String statusTopic = "mesh/node/" + nodeID + "/status";
        String commandTopic = "mesh/node/" + nodeID + "/command";

        if (client.connect(
                "T3S3_Gateway",
                statusTopic.c_str(),
                1,
                true,
                "offline")) {

            Serial.println("MQTT Connected");

            client.publish(statusTopic.c_str(), "online", true);

            // Subscribe to command topic
            client.subscribe(commandTopic.c_str());
            Serial.println("Subscribed to command topic");
        } 
        else {
            Serial.print("MQTT Failed, rc=");
            Serial.println(client.state());
        }

        lastReconnectAttempt = millis();
    }
}

// ================= PUBLISH SENSOR =================
bool MQTTGateway::publishSensor(float temp, float humidity) {

    if (RoleController::getRole() != ROLE_SINK) return false;
    if (WiFi.status() != WL_CONNECTED) return false;
    if (!client.connected()) return false;

    String nodeID = NodeIdentity::getNodeID();
    String topic = "mesh/node/" + nodeID + "/sensor";

    String payload = "{";
    payload += "\"node\":\"" + nodeID + "\",";
    payload += "\"temperature\":" + String(temp) + ",";
    payload += "\"humidity\":" + String(humidity) + ",";
    payload += "\"timestamp\":" + String(millis());
    payload += "}";

    return client.publish(topic.c_str(), payload.c_str(), false);
}