#pragma once

namespace RC::Network
{
    // Wifi
    const char *ssid = "****************";
    const char *password = "****************";
    const char *serverHostname = "test.mosquitto.org";

    const char *homeTopic = "home/message";

    const char *controls = "controls";
    const char *controlSteeringTopic = "controls/steering";
    const char *controlThrottleTopic = "controls/throttle";

    const char *sensors = "sensors";
    const char *sensorEngine = "sensors/first";
    const char *sensorController = "sensors/second";
}
