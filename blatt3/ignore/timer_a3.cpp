#include <Arduino.h>
#include <DueTimer.h>


const uint32_t t3_frequency = 1000;
const uint32_t t4_frequency = 1000;

DueTimer Timer3;
DueTimer Timer4;

int start3;
int start4;
int counter = 0;
bool flop3 = false;
bool flop4 = false;
bool led = false;
const int button_press_length = 40;

void timer3ISR() {
    if (!flop3 || millis() - start3 > button_press_length) {
        start3 = millis();
        flop3 = false;
    }
    Timer3.start();
}

void timer4ISR() {
    Serial.println("timer4ISR");
    if (!flop4 || millis() - start4 > button_press_length) {
        start4 = millis();
        flop4 = false;
    }
    Timer4.start();
}

void timer3Routine() {
    int button = digitalRead(3);
    //Serial.println(button == HIGH ? "HIGH" : "LOW");
    if (button == HIGH) {
        int interval = millis() - start3;
        if (interval >= button_press_length) {
            Timer3.stop();
            Serial.println(++counter);
            led = !led;
        } else {
            flop3 = true;
            Timer3.stop();
        }
    }
}

void timer4Routine() {
    int button = digitalRead(5);
    //Serial.println(button == HIGH ? "HIGH" : "LOW");
    if (button == HIGH) {
        int interval = millis() - start4;
        if (interval >= button_press_length) {
            Timer4.stop();
            Serial.println(++counter);
            led = !led;
        } else {
            flop4 = true;
            Timer4.stop();
        }
    }
}

void setup() {
    Serial.begin(9600);

    pinMode(3, INPUT);
    pinMode(34, INPUT);

    pinMode(13, OUTPUT);

    attachInterrupt(digitalPinToInterrupt(3), timer3ISR, FALLING);
    attachInterrupt(digitalPinToInterrupt(34), timer4ISR, FALLING);


    if (!Timer3.configure(t3_frequency, timer3Routine)) Serial.println("timer3 configure failed");
    //if (!Timer4.configure(t4_frequency, timer4Routine)) Serial.println("timer4 configure failed");
}

void loop() {
    digitalWrite(13, led);
}