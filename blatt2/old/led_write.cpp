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

void error(std::string input) {
	Serial.println("Invalid command");
	if (input == "illuminance") {
		Serial.println("Usage: illuminance <val> set LED luminance to val in %; values 0 - 100 allowed");
	} else if (input.replace(2, -1, "") == "led") {
			Serial.println("Usage:  led <on|off> turn LED on or off");
	} else {
		help();
	}
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
		error();
	} else {
		luminance = lum * 2.55;
		analogWrite(13, luminance);
	}
}

void loop()
{
	std::string input = "";
	int next = 0;
	while (Serial.available() > 0 && next < 63) {
		input.append(1, Serial.read());
	}

	Serial.println("");

	std::regex illuminance_regex("^illuminance \\d{1,3}$");

	switch (input) {
		case "help": help(); break;
		case "led on": LEDon(); break;
		case "led off": LEDoff(); break;
		default: {
			if (std::regex_match(input, illuminance_regex)) {
				std::string replaced = input.replace(0, 12, "");
				illuminance(std::stoi(replaced));
			}
			else {
				error("illuminance");
			}
		}
	}
}
