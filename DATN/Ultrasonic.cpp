#include "Ultrasonic.h"

Ultrasonic::Ultrasonic(uint8_t triggerPin, uint8_t echoPin) {
    _triggerPin = triggerPin;
    _echoPin = echoPin;

    pinMode(_triggerPin, OUTPUT);
    pinMode(_echoPin, INPUT);
}

float Ultrasonic::distanceRead() {
    // Clear the trigger pin
    digitalWrite(_triggerPin, LOW);
    delayMicroseconds(2);

    // Set the trigger pin to HIGH for 10 microseconds
    digitalWrite(_triggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(_triggerPin, LOW);

    // Read the echo pin
    unsigned long duration = pulseIn(_echoPin, HIGH);

    // Calculate the distance in cm
    float distance = (duration * 0.0343) / 2;

    return distance;
}
