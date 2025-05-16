#include <Arduino.h>
#include <DueTimer.h>

/*
 *flop rausnehmen
 *button soll 40ms low sein dann 40ms high
 *nicht millis() sondern timer durchläufe zählen
 */

const uint32_t t4_frequency = 1000;

DueTimer Timer4;

long start=0;
int counter = 0;
bool flop = false;
bool led = false;
const int button_press_length = 40;

void timerISR() {
    if (!flop || (millis() - start) > button_press_length) {
        start = millis();
        flop = false;
    }
    Timer4.start();
}

void timerRoutine() {
    int button = digitalRead(3);
    //Serial.println(button == HIGH ? "HIGH" : "LOW");
    if (button == HIGH) {
        int interval = millis() - start;
        if (interval >= button_press_length) {
            Timer4.stop();
            Serial.println(++counter);
            led = !led;
        } else {
            flop = true;
            Timer4.stop();
        }
    }
}

void setup() {
    Serial.begin(9600);

    pinMode(3, INPUT);

    pinMode(13, OUTPUT);

    attachInterrupt(digitalPinToInterrupt(3), timerISR, FALLING);

    if (Timer4.configure(t4_frequency, timerRoutine)) {

    } else {
        Serial.println("Failed to configure Timer4");
    }
}

void loop() {
    digitalWrite(13, led);
}