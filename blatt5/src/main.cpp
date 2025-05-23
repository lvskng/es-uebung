#include <Arduino.h>
#include <Servo.h>
#include <DueTimer.h>

Servo servo1;
Servo servo2;

DueTimer Timer3;
DueTimer Timer4;
DueTimer Timer5;

const int button_press_length = 40;

int positions[64];
uint positions_count = 0;

int j_x_pos = 0;
int j_y_pos = 0;

bool led = false;

int button12counter;
bool button12_low;
bool button12;
void timer5ISR() {
    button12counter = 0;
    Timer5.start();
    Serial.println("button 12 low");
}

void button12_pressed() {
    button12_low = false;
    Timer5.stop();
    button12 = true;
    Serial.println("button 12 pressed");
    positions[positions_count++] = j_x_pos;
    positions[positions_count++] = j_y_pos;
    if (positions_count == 63) {
        positions_count = 0;
        Serial.print("max positions recorded");
    }
}

void timer5Routine() {
    Serial.println("timer5 routine");
    int button = digitalRead(12);
    //Serial.println(button == HIGH ? "HIGH" : "LOW");
    if (button == LOW || button12_low && button == HIGH) {
        button12counter += 10;
    } else {
        Timer5.stop();
        //Serial.println("Faulty press 3");
    }
    if (button12counter >= button_press_length) {
        if (button12_low) {
            button12_pressed();
        } else {
            button12counter = 0;
            button12_low = true;
        }
    }
}

bool button11_low;
uint32_t button11counter;
bool button11 = false;

void timer4ISR() {
    button11counter = 0;
    Timer4.start();
    Serial.println("button 11 low");
}

void button4_pressed() {
    Serial.println("button 4 pressed");
    button11_low = false;
    Timer4.stop();
    button11 = true;
    for (int i = 0; i < 64; i++) {
        Serial.print("[");
        Serial.print(positions[i]);
        Serial.print(positions[++i]);
        Serial.print("], ");
    }
}

void timer4Routine() {
    Serial.println("timer4 routine");
    int button = digitalRead(4);
    //Serial.println(button == HIGH ? "HIGH" : "LOW");
    if (button == LOW || button11_low && button == HIGH) {
        button11counter += 10;
    } else {
        Timer4.stop();
    }
    if (button11counter >= button_press_length) {
        if (button11_low) {
            button4_pressed();
        } else {
            button11counter = 0;
            button11_low = true;
        }
    }
}

void timer_routine() {
    uint32_t joystick_x = analogRead(A8);
    uint32_t joystick_y = analogRead(A9);
    float jx_abs = (float)(float(joystick_x) - 512) / 512;
    float jy_abs = (float)(float(joystick_y) - 512) / 512;

    if (jx_abs == 0 && jy_abs == 0) {
        Timer3.stop();
    }

    j_x_pos += int(jx_abs * 18);
    j_y_pos += int(jy_abs * 18);
}

void timer3_start_isr() {
    Timer3.start();
}

void led_toggle() {
    led = !led;
}

void setup() {
    //pinMode(7, OUTPUT); //Servo 1 pwm
    //pinMode(6, OUTPUT); //Servo 2 pwm
    Serial.begin(9600);

    servo1.attach(7);
    servo2.attach(6);

    pinMode(11, INPUT); //Button 1
    pinMode(12, INPUT); //Button 2
    pinMode(18, INPUT);
    pinMode(27, OUTPUT);

    pinMode(A8, INPUT); //Joystick XOUT
    pinMode(A9, INPUT); //Joystick YOUT

    if (!Timer3.configure(10, timer_routine) || !Timer4.configure(100, timer4Routine) || !Timer5.configure(100, timer5Routine)) {
        Serial.println("Timer configuration failed");
    };

    attachInterrupt(digitalPinToInterrupt(A8), timer3_start_isr, RISING);
    attachInterrupt(digitalPinToInterrupt(A9), timer3_start_isr, RISING);
    attachInterrupt(digitalPinToInterrupt(18), led_toggle, FALLING);
    attachInterrupt(digitalPinToInterrupt(11), timer4ISR, FALLING);
    attachInterrupt(digitalPinToInterrupt(12), timer5ISR, FALLING);
}

void loop() {
    if (j_x_pos > 180) {
        j_x_pos = 180;
    }
    if (j_y_pos > 180) {
        j_y_pos = 180;
    }
    if (j_x_pos < 0) {
        j_x_pos = 0;
    }
    if (j_y_pos < 0) {
        j_y_pos = 0;
    }

    servo1.write(j_x_pos);
    servo2.write(j_y_pos);

    digitalWrite(27, led ? HIGH : LOW);
}