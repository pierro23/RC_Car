#include "RC_Controller.hpp"

using namespace RC;

Controller controller;

void setup(void)
{
    // pinMode(Pin::LedPin, OUTPUT);
    // pinMode(Pin::ButtonPin, INPUT);

    // digitalWrite(Pin::LedPin, HIGH);
    controller.setup();
}

void loop(void)
{
    controller.run();
    // delay(200);
}
