#include <Arduino.h>
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
	pinMode(A9, INPUT);
}

int stick_value;

void loop()
{
	int read = analogRead(A9);
	if (read < stick_value - 1 || read > stick_value + 1) {
		stick_value = read;
		float voltage = read * (3.3 / 1023);
		Serial.print("pin A9 - ADC-value:");
		print_float(read, 0);
		Serial.print("; input voltage: ");
		print_float(voltage, 2);
		Serial.print(" V, ");
		print_float(voltage * 1000, 0);
		Serial.print(" mV\n");
	}
	delay(100);
}
