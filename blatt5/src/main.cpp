#include <Arduino.h>
#include <Servo.h>
#include <DueTimer.h>

Servo servo1;
Servo servo2;

DueTimer Timer3;
DueTimer Timer1;
DueTimer Timer7;
DueTimer Timer8;

const int button_press_length = 40;

const uint max_positions = 32;

int positions[max_positions];
int positions_count = 0;

int j_x_pos = 0;
int j_y_pos = 0;

bool led = false;

bool live_mode = true;

void button12_pressed() {
    if (positions_count == max_positions - 1) {
        Serial.print("max positions recorded");
    } else {
        positions[positions_count++] = j_x_pos;
        positions[positions_count++] = j_y_pos;
        Serial.println("Position recorded");
    }
}

void clear_positions() {
    positions_count = 0;
}

int replay_timer_i;
int target_pos_x;
int target_pos_y;
double x_step;
double y_step;
double nextx;
double nexty;
const int replay_rate = 100;
int counter = 0;
bool initial_position_reached = false;
void replay_timer_routine() {
    if (!initial_position_reached) {
        if (abs(servo1.read() - positions[0]) < 2 && abs(servo2.read() - positions[1]) < 2) {
            initial_position_reached = true;
        } else {
            return;
        }
    }
    if (replay_timer_i >= positions_count) {
        Timer8.stop();
        live_mode = true;
        Serial.println("Live mode");
    } else {
        if (counter == replay_rate) {
            replay_timer_i += 2;
            target_pos_x = positions[replay_timer_i];
            target_pos_y = positions[replay_timer_i + 1];
            x_step = double(servo1.read() - target_pos_x) / replay_rate;
            y_step = double(servo2.read() - target_pos_y) / replay_rate;
            Serial.print(x_step);
            Serial.print(" ");
            Serial.println(y_step);
            counter = 0;
        }
        nextx += x_step;
        nexty += y_step;
        while (abs(nextx) >= 1) {
            j_x_pos += 1;
            nextx -= nextx > 0 ? 1 : -1;
        }
        while (abs(nexty) >= 1) {
            j_y_pos += 1;
            nexty -= nexty > 0 ? 1 : -1;
        }
        counter++;
    }
}

void button11_pressed() {
    Serial.println("Replay mode");
    if (positions_count == 0) {
        Serial.println("No positions recorded");
    }
    for (int i = 0; i < positions_count; i++) {
        Serial.print("[");
        Serial.print(positions[i]);
        Serial.print(", ");
        Serial.print(positions[++i]);
        Serial.print("], ");
    }
    j_x_pos = positions[0];
    j_y_pos = positions[1];
    if (positions_count >= 2) {
        initial_position_reached = false;
        replay_timer_i = 2;
        counter = 0;
        live_mode = false;
        target_pos_x = positions[replay_timer_i];
        target_pos_y = positions[replay_timer_i + 1];
        x_step = double(servo1.read() - target_pos_x) / replay_rate;
        y_step = double(servo2.read() - target_pos_y) / replay_rate;
        nextx = 0;
        nexty = 0;
        Timer8.start();
    }
}


void timer_routine() {
    uint32_t joystick_x = analogRead(A8);
    uint32_t joystick_y = analogRead(A9);
    float jx_abs = (float)(float(joystick_x) - 512) / 512;
    float jy_abs = (float)(float(joystick_y) - 512) / 512;

    if (!(abs(joystick_x - 512) < 128 && abs(joystick_y - 512) < 128)) {
        if (live_mode) {
            j_x_pos += int(jx_abs * 18);
            j_y_pos += int(jy_abs * 18);
        }
    } else {
        Timer3.stop();
    }

}

void timer3_start_isr() {
    Timer3.start();
}

void led_toggle() {
    led = !led;
}

uint button11 = 0;
bool button11_low = false;
uint button12 = 0;
bool button12_low = false;
uint sel = 0;
bool sel_low = false;

void check_button11() {
    int button11_state = digitalRead(11);
    if (button11_state == LOW) {
        button11 += 10;
        button11_low = true;
    } else if (button11_state == HIGH && button11_low) {
        button11_low = false;
        button11 += 10;
    }
    else {
        button11_low = false;
        button11 = 0;
    }

}

void check_button12() {
    int button12_state = digitalRead(12);
    if (button12_state == LOW ) {
        button12 += 10;
        button12_low = true;
    } else if (button12_state == HIGH && button12_low) {
        button12_low = false;
        button12 += 10;
    }
    else {
        button12_low = false;
        button12 = 0;
    }

}

void check_sel() {
    int sel_state = digitalRead(18);
    if (sel_state == LOW) {
        sel_low = true;
        sel += 10;
    } else if (sel_state == HIGH && sel_low) {
        sel_low = false;
        sel += 10;
    }
    else {
        sel_low = false;
        sel = 0;
    }
}

void button_timer_routine() {
    if (button11_low || button11 > 0) {
        check_button11();

    }
    if (button12_low || button12 > 0) {
        check_button12();
    }
    if (sel_low || sel > 0) {
        check_sel();
    }

    if (sel >= button_press_length) {
        Timer7.stop();

        sel = 0;
        sel_low = false;
        button11 = 0;
        button12 = 0;
        button11_low = false;
        button12_low = false;
        led_toggle();
        return;
    }
    if (button11 >= button_press_length && button12 >= button_press_length) {
        Timer7.stop();

        button11 = 0;
        button11_low = false;
        button12 = 0;
        button12_low = false;
        clear_positions();
    } else if (button11 >= button_press_length) {
        Timer7.stop();

        button11 = 0;
        button11_low = false;
        button11_pressed();
    } else if (button12 >= button_press_length) {
        Timer7.stop();

        button12 = 0;
        button12_low = false;
        button12_pressed();
    }
}

void button_isr() {
    if (digitalRead(11) == LOW) {
        button11_low = true;
    }
    if (digitalRead(12) == LOW) {
        button12_low = true;
    }
    if (digitalRead(18) == LOW) {
        sel_low = true;
    }
    Timer7.start();
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

    if (!Timer3.configure(10, timer_routine) || !Timer7.configure(100, button_timer_routine) || !Timer8.configure(replay_rate, replay_timer_routine)) {
        Serial.println("Timer configuration failed");
    };

    attachInterrupt(digitalPinToInterrupt(A8), timer3_start_isr, RISING);
    attachInterrupt(digitalPinToInterrupt(A9), timer3_start_isr, RISING);
    attachInterrupt(digitalPinToInterrupt(18), led_toggle, FALLING);
    attachInterrupt(digitalPinToInterrupt(11), button_isr, FALLING);
    attachInterrupt(digitalPinToInterrupt(12), button_isr, FALLING);
    //timer3_start_isr();
}

int lastx;
int lasty;

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

/*
*  if (abs(j_x_pos - lastx) > 5) {
servo1.write(j_x_pos);
lastx = j_x_pos;
} else {
servo1.write(lastx);
}
if (abs(j_y_pos - lasty) > 5) {
servo2.write(j_y_pos);
lasty = j_y_pos;
} else {
servo2.write(lasty);
}*/