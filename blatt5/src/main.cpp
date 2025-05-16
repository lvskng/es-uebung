#include <Arduino.h>
#include <Servo.h>

Servo servo1;
Servo servo2;

void setup() {
    //pinMode(7, OUTPUT); //Servo 1 pwm
    //pinMode(6, OUTPUT); //Servo 2 pwm

    servo1.attach(7);
    servo2.attach(6);

    pinMode(4, INPUT); //Button 1
    pinMode(3, INPUT); //Button 2

    pinMode(A8, INPUT); //Joystick XOUT
    pinMode(A9, INPUT); //Joystick YOUT

}

void loop() {
    uint32_t joystick_x = analogRead(A8);
    uint32_t joystick_y = analogRead(A9);

    servo1.write((joystick_x / 255) * 180);
    servo2.write((joystick_y / 255) * 180);
}