#include <Arduino.h>

#include <CommandParser.h>
CommandParser commandParser;

void setup()
{
	Serial.begin(115200);

	commandParser.delimiter = ' '; // Set space as delimiter (default is ',')

	commandParser.add_command("TEST", [](ScanUtil* scan) {
		Serial.println(F("TEST command executed"));
    });

	commandParser.add_command("SET", [](ScanUtil* scan) {
		if(scan->seekIf("SPEED="))
		{
			int speed;
			if(scan->get_int(&speed))
			{
				Serial.print(F("speed="));
				Serial.println(speed);
			}
			else
				Serial.println(F("Err: SET SPEED Missing or invalid speed value"));
		}
		else if(scan->seekIf("ACCEL="))
		{
			int accel;
			if(scan->get_int(&accel))
			{
				Serial.print(F("accel="));
				Serial.println(accel);
			}
			else
				Serial.println(F("Err: SET ACCEL Missing or invalid acceleration value"));
		}
		else
		{
			Serial.println(F("Err: unknown SET command parameter"));
		}
	});

	commandParser.on_error([](char* error_message) {
		Serial.print(F("Err: "));
		Serial.println(error_message);
	});

}

void loop()
{	
	if(Serial.available())
	{
		char c = Serial.read();
		Serial.write(c); // Echo back the received character
		commandParser.processInput(c);
	}
}