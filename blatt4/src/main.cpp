#include <Arduino.h>
#include <DueTimer.h>

const uint32_t t3_frequency = 100;
const uint32_t t4_frequency = 100;
const uint32_t t5_frequency = 25;

bool button3 = false;
bool button4 = false;

const uint32_t button_press_length = 40;

DueTimer Timer3;
DueTimer Timer4;
DueTimer Timer5;

bool button3_low;
uint32_t button3counter;

byte led_red_status = LOW;
byte led_green_status = LOW;
byte led_blue_status = LOW;

void led_red() {
    led_red_status = !led_red_status;
}

void led_green() {
    led_green_status = HIGH;
    led_blue_status = LOW;
}

void led_blue() {
    led_green_status = LOW;
    led_blue_status = HIGH;
}

void timer3ISR() {
    button3counter = 0;
    Timer3.start();
}

void button3_pressed() {
    button3_low = false;
    Timer3.stop();
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
        //Serial.println("Faulty press 3");
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

enum motor_state {
    Counterclockwise,
    Stop,
    Clockwise,
};

motor_state current_motor_state = Stop;

uint32_t motor_power = 0;

const bool ROTATION_MODE = true;
const bool POWER_MODE = false;

bool mode = ROTATION_MODE;

void set_led_by_mode() {
    if (mode == ROTATION_MODE) {
        led_green();
    } else if (mode == POWER_MODE) {
        led_blue();
    }
}

void double_press() {
    mode = !mode;
    set_led_by_mode();
}

void clockwise() {
    if (current_motor_state != Stop) {
        current_motor_state = Stop;
    } else {
        current_motor_state = Clockwise;
        set_led_by_mode();
    }
    led_red();
}

void counter_clockwise() {
    if (current_motor_state != Stop) {
        current_motor_state = Stop;
    } else {
        current_motor_state = Counterclockwise;
        set_led_by_mode();
    }
    led_red();
}

void power_up() {
    motor_power += 5;
    if (motor_power > 100) {
        motor_power = 100;
    }
}
void power_down() {
    if (motor_power >= 5) {
        motor_power = motor_power - 5;
    } else {
        motor_power = 0;
    }
}

void print_state() {
    Serial.print(mode ? "[DIRECTION] " : "[POWER] ");
    Serial.print("Motor direction: ");
    for (int i = Counterclockwise; i <= Clockwise; i++) {
        String mode_string = "";
        switch (i) {
            case Clockwise:
                mode_string = "Clockwise";
                break;
            case Counterclockwise:
                mode_string = "Counterclockwise";
                break;
            case Stop:
                mode_string = "Stop";
                break;
        }
        if (current_motor_state == i) {
            Serial.print(" (");
            mode_string += ")";
        }
        Serial.print(mode_string + " ");
    }
    Serial.print("| Motor power: ");
    Serial.print(motor_power);
    Serial.println("%");
}

void timer5Routine() {
    if (button3 && button4) {
        double_press();
    } else if (button3) {
        if (mode == ROTATION_MODE) {
            counter_clockwise();
        } else {
            power_up();
        }
    } else if (button4) {
        if (mode == ROTATION_MODE) {
            clockwise();
        } else {
            power_down();
        }
    }
    print_state();
    button3 = false;
    button4 = false;
    Timer5.stop();
}

void setup() {
    Serial.begin(9600);

    pinMode(3, INPUT);
    pinMode(4, INPUT);

    pinMode(6, OUTPUT);
    pinMode(7, OUTPUT);
    pinMode(8, OUTPUT); //pwm
    pinMode(11, OUTPUT);
    //pinMode(13, OUTPUT);

    pinMode(49, OUTPUT); //in 1
    pinMode(47, OUTPUT); //in 2

    attachInterrupt(digitalPinToInterrupt(3), timer3ISR, FALLING);
    attachInterrupt(digitalPinToInterrupt(4), timer4ISR, FALLING);

    if (Timer4.configure(t4_frequency, timer4Routine) && Timer3.configure(t3_frequency, timer3Routine) && Timer5.configure(t5_frequency, timer5Routine)) {

    } else {
        Serial.println("Failed to configure Timer");
    }
}

void loop() {
    digitalWrite(11, led_red_status);
    digitalWrite(7, led_green_status);
    digitalWrite(6, led_blue_status);
    switch (current_motor_state) {
        case Stop:
            digitalWrite(49, LOW);
            digitalWrite(47, LOW);
            break;
        case Clockwise:
            digitalWrite(49, HIGH);
            digitalWrite(47, LOW);
            break;
        case Counterclockwise:
            digitalWrite(49, LOW);
            digitalWrite(47, HIGH);
            break;
    }
    analogWrite(8, motor_power * (255/100));
}