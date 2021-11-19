#pragma once

#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <Servo.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <FirebaseESP8266.h>

#include "Network.hpp"
#include "FirebaseCredential.hpp"
#include "Pin.hpp"

constexpr auto DebugOut { false };

namespace RC
{
    struct Controller;
}

struct RC::Controller
{
    WiFiClient wifiClient;
    PubSubClient mqttClient;
    Servo servoSteering;
    Servo servoThrottle;
    OneWire temperatureWire;
    DallasTemperature temperatureSensor;
    FirebaseData firebaseData;
    FirebaseJsonData jsonData;

    Controller(void) : mqttClient(wifiClient) {}

    void setupWifi(void) noexcept
    {
        // Wifi
        delay(100);
        if constexpr (DebugOut) {
            Serial.printf("\nConnecting to %s\n", Network::ssid);
        }
        WiFi.mode(WIFI_STA);
        WiFi.begin(Network::ssid, Network::password);

        while (WiFi.status() != WL_CONNECTED) {
            delay(250);
            if constexpr (DebugOut) {
                Serial.print(".");
            }
        }
        if constexpr (DebugOut) {
            Serial.println("");
            Serial.print("WiFi connected on IP address ");
            Serial.println(WiFi.localIP());
        }

        // Firebase
        Firebase.begin(Credentials::host, Credentials::auth);
        if constexpr (DebugOut) {
            if (Firebase.ready())
                Serial.print("ready");
            else
                Serial.print("not ready");
        }
    }

    void setup(void) noexcept
    {
        if constexpr (DebugOut)
            Serial.begin(115200);
        // Wifi
        setupWifi();

        // MQTT
        // mqttClient.setServer(Network::serverHostname, 1883);
        // mqttClient.setCallback([this] (char *topic, uint8_t *payload, unsigned int length) -> void
        //     { callback(topic, payload, length); }
        // );

        // Servo
        servoSteering.attach(Pin::SteeringPin);
        servoThrottle.attach(Pin::ThrottlePin);
        servoSteering.write(0);
        delay(500);
        servoSteering.write(90);
        delay(500);
        servoSteering.write(180);
        delay(500);
        servoSteering.write(90);

        // Temp sensors
        temperatureWire.begin(Pin::TemperaturePin);
        temperatureSensor.setOneWire(&temperatureWire);
        temperatureSensor.begin();
        temperatureSensor.setWaitForConversion(false);
    }

    void updateControls(void) noexcept
    {
        Firebase.getJSON(firebaseData, Network::controls);
        firebaseData.jsonObject().get(jsonData, "steering");
        servoSteering.write(jsonData.intValue);
        if constexpr (DebugOut) {
            Serial.print("steering: ");
            Serial.println(jsonData.intValue);
        }
        firebaseData.jsonObject().get(jsonData, "throttle");
        servoThrottle.write(jsonData.intValue);
        if constexpr (DebugOut) {
            Serial.print("throttle: ");
            Serial.println(jsonData.intValue);
        }
    }

    void updateSensors(void) noexcept
    {
        temperatureSensor.requestTemperatures();
        auto temp = temperatureSensor.getTempCByIndex(0);
        Firebase.setDouble(firebaseData, Network::sensorEngine, temp);
        if constexpr (DebugOut) {
            Serial.print(temp);
            Serial.println("°C");
        }
        temp = temperatureSensor.getTempCByIndex(1);
        Firebase.setDouble(firebaseData, Network::sensorController, temp);
        if constexpr (DebugOut) {
            Serial.print(temp);
            Serial.println("°C");
            Serial.println();
        }
    }

    void connectWebServer(void) noexcept
    {
        while (!mqttClient.connected()) {
            // Create a random mqttClient ID
            String clientId = "ESP8266-";
            clientId += String(random(0xffff), HEX);
            if constexpr (DebugOut) {
                Serial.printf("MQTT connecting as mqttClient %s...\n", clientId.c_str());
            }

            // Attempt to connect
            if (mqttClient.connect(clientId.c_str())) {
                if constexpr (DebugOut)
                    Serial.println("MQTT connected");
                // Once connected, publish an announcement...
                mqttClient.publish(Network::homeTopic, "hello from ESP8266");
                // ... and resubscribe
                mqttClient.subscribe(Network::homeTopic);
                mqttClient.subscribe(Network::controlSteeringTopic);
            } else {
                if constexpr (DebugOut) {
                    Serial.printf("MQTT failed, state %i, retrying...\n", mqttClient.state());
                }
                // Wait before retrying
                delay(2500);
            }
        }
    }

    void callback(char *topic, byte *payload, unsigned int length)
    {
        // copy payload to a static string
        String msg(length);
        strncpy(msg.begin(), reinterpret_cast<char *>(payload), length);

        if constexpr (DebugOut) {
            Serial.printf("topic %s, msg: %s\n", topic, msg.c_str());
        }

        const String top(topic);
        if (top == Network::homeTopic) {
        } else if (top == Network::controlSteeringTopic) {
            servoSteering.write(msg.toInt());
        } else if (top == Network::controlThrottleTopic) {
        } else {
            if constexpr (DebugOut) {
                Serial.print("Unknown topic: ");
                Serial.println(topic);
            }
        }
    }

    void run(void)
    {
        // MQTT (old version)
        // connectWebServer();
        // mqttClient.loop();

        // Firebase controls
        updateControls();

        // Temperature sensors
        updateSensors();
    }
};
