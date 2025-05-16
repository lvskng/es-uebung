#include <Arduino.h>
#include <DueTimer.h>


const uint32_t t4_frequency = 50;

DueTimer Timer4;

int start;

void timerISR() {
    start = millis();
    Timer4.start();
}

void timerRoutine() {
    int button = digitalRead(3);
    Serial.println(button == HIGH ? "HIGH" : "LOW");
    if (button == HIGH) {
        Timer4.stop();
        Serial.println(millis() - start);
    }
}

void setup() {
    Serial.begin(9600);

    pinMode(3, INPUT);

    attachInterrupt(digitalPinToInterrupt(3), timerISR, FALLING);

    if (Timer4.configure(t4_frequency, timerRoutine)) {

    } else {
        Serial.println("Failed to configure Timer4");
    }
    Timer4.start();
}

void loop() {

}