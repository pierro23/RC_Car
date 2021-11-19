#pragma once

namespace RC::Pin
{
    /**
     * D0 = GPIO16
     * D5 = GPIO14
     * D6 = GPIO12
     * D7 = GPIO13
     * D8 = GPIO15
     *
     * D1 = GPIO5
     * D2 = GPIO4
     * D3 = GPIO0
     * D4 = GPIO2
     */

    static const int LedPin = 13;
    static const int SteeringPin = 0; // D3
    static const int ThrottlePin = 2;

    static const int TemperaturePin = 14; // D2
}
