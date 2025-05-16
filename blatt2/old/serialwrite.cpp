#include <Arduino.h>
char ch_buffer[80] = "";
float pi_fl = 3.14159265359;
void ftoa(float num, char* res, int numDecPlaces)
{
//code of ftoa sprintf
}
void print_float(float num, int numDecPlaces) {
	Serial.println(num, numDecPlaces);
}
void setup()
{
// Initialisierung der ersten seriellen UART-Schnittstelle (9600 Baud)
Serial.begin(9600);
Serial.println("Test␣print␣Pi\n");
Serial.print("print␣Pi␣using␣print_float:␣");
print_float(pi_fl , 3);
Serial.println("␣");
Serial.print("print␣Pi␣using␣ftoa:␣");
ftoa(pi_fl , ch_buffer , 5);
Serial.println(ch_buffer);
Serial.print("\ncross␣check:␣Serial.println(pi_ch ,␣5):\t");
Serial.println(pi_fl , 5);
}
void loop()
{
//code of mainloop
}
