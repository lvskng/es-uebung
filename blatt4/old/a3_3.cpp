#include <Arduino.h>
#include <DueTimer.h>

const uint32_t t3_frequency = 100;
const uint32_t t4_frequency = 100;
const uint32_t t5_frequency = 25;

bool led = true;
bool button3 = false;
bool button4 = false;

const uint32_t button_press_length = 40;

DueTimer Timer3;
DueTimer Timer4;
DueTimer Timer5;

bool button3_low;
uint32_t button3counter;

void timer3ISR() {
    button3counter = 0;
    Timer3.start();
}

void button3_pressed() {
    button3_low = false;
    Timer3.stop();
    Serial.println("Button 3 pressed");
    led = !led;
    button3 = true;
    Timer5.start();
}

void timer3Routine() {
    int button = digitalRead(3);
    //Serial.println(button == HIGH ? "HIGH" : "LOW");
    if (button == LOW || button3_low && button == HIGH) {
        button3counter += 10;
    } else {
        Timer3.stop();
        Serial.println("Faulty press 3");
    }
    if (button3counter >= button_press_length) {
        if (button3_low) {
            button3_pressed();
        } else {
            button3counter = 0;
            button3_low = true;
        }
    }
}

bool button4_low;
uint32_t button4counter;

void timer4ISR() {
    button4counter = 0;
    Timer4.start();
}

void button4_pressed() {
    button4_low = false;
    Timer4.stop();
    Serial.println("Button 4 pressed");
    led = !led;
    button4 = true;
    Timer5.start();
}

void timer4Routine() {
    int button = digitalRead(4);
    //Serial.println(button == HIGH ? "HIGH" : "LOW");
    if (button == LOW || button4_low && button == HIGH) {
        button4counter += 10;
    } else {
        Timer4.stop();
        Serial.println("Faulty press 4");
    }
    if (button4counter >= button_press_length) {
        if (button4_low) {
            button4_pressed();
        } else {
            button4counter = 0;
            button4_low = true;
        }
    }
}

uint double_press_counter = 0;

void double_press() {
    Serial.print(++double_press_counter);
    Serial.println(" double presses");
}

void timer5Routine() {
    if (button3 && button4) {
        double_press();
    }
    button3 = false;
    button4 = false;
    Timer5.stop();
}

void setup() {
    Serial.begin(9600);

    pinMode(3, INPUT);
    pinMode(4, INPUT);

    pinMode(13, OUTPUT);

    attachInterrupt(digitalPinToInterrupt(3), timer3ISR, FALLING);
    attachInterrupt(digitalPinToInterrupt(4), timer4ISR, FALLING);

    if (Timer4.configure(t4_frequency, timer4Routine) && Timer3.configure(t3_frequency, timer3Routine) && Timer5.configure(t5_frequency, timer5Routine)) {

    } else {
        Serial.println("Failed to configure Timer4");
    }
}

void loop() {
    digitalWrite(13, led);
}