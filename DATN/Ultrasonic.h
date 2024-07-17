#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <Arduino.h>

class Ultrasonic {
public:
    Ultrasonic(uint8_t triggerPin, uint8_t echoPin);  // Constructor to initialize the Ultrasonic object with specified pins                                // Initialize the pins
    float distanceRead();                             // Method to read the distance from the Ultrasonic sensor

private:
    uint8_t _triggerPin;  // Pin where the trigger is connected
    uint8_t _echoPin;     // Pin where the echo is connected
};

#endif
