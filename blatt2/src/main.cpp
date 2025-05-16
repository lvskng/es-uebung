#include <Arduino.h>
#include <regex>
#include <string>
#include <sys/_intsup.h>
char ch_buffer[80] = "";
float pi_fl = 3.14159265359;
void ftoa(float num, char* res, int numDecPlaces)
{
	//code of ftoa
}
void print_float(float num, int numDecPlaces) {
	Serial.print(num, numDecPlaces);
}

void setup()
{
	// Initialisierung der ersten seriellen UART-Schnittstelle (9600 Baud)
	Serial.begin(9600);
	pinMode(13, OUTPUT);
}

void help() {
	Serial.println("Available commands:");
	Serial.println("  help - show this help");
	Serial.println("  led <on|off> turn LED on or off");
	Serial.println("  illuminance <val> set LED luminance to val in %; values 0 - 100 allowed");
}

bool led;
int luminance;

void LEDon() {
	analogWrite(13, luminance);
}

void LEDoff() {
	digitalWrite(13, LOW);
}

void illuminance(uint lum) {
	if (lum > 100) {
		Serial.println("Illuminance can only be set to a value between 0 and 100");
		Serial.println("Usage: illuminance <val> set LED luminance to val in %; values 0 - 100 allowed");
	} else {
		luminance = lum * 2.55;
		analogWrite(13, luminance);
	}
}

void loop()
{
	char input[64] = "";
	int next = 0;
	while (Serial.available() > 0 && next < 63) {
		input[next++] = Serial.read();
	}
	input[next] = '\0';

	Serial.println(input);

	char *ptr = strtok(input, " ");

	String commmand = String(ptr);

	switch (commmand) {
		case "help": help(); break;
		case "led": {
			String mode = String(ptr);
			if (mode == "on") {
				LEDon();
			} else if (mode == "off") {
				LEDoff();
			} else {
				Serial.println("Invalid mode " + mode);
				Serial.println("Usage:  led <on|off> turn LED on or off");
			}
			break;
		}
		case "illuminance": illuminance(atoi(ptr)); break;
		default: {
			Serial.println("Invalid command " + commmand);
			help();
		}
	}
}
