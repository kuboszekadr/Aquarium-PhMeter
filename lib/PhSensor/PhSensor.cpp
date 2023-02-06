#include "PhSensor.h"

Sensors::Measures Sensors::measure[1] = {Sensors::Measures::PH};

Sensors::PhSensor::PhSensor(
	uint8_t pin,
	uint8_t id_sensor,
	const char *name)
	: Sensor(
		  id_sensor,
		  measure,
		  1,
		  name,

		  Events::EventType::PH_LOW,
		  Events::EventType::PH_HIGH)
{
	_pin = pin;
}

bool Sensors::PhSensor::makeReading()
{
	float _neutralVoltage = 1500.0;
	float _acidVoltage = 2032.44;

	float slope = (7.0 - 4.0) / ((1500.0 - 1500.0) / 3.0 - (_acidVoltage - 1500.0) / 3.0);
	float intercept = 7.0 - slope * (_neutralVoltage - 1500.0) / 3.0;

	float voltage = analogRead(this->_pin) / ESPADC * ESPVOLTAGE;
	float ph = slope * (voltage - 1500.0) / 3.0 + intercept;
	// float voltage = analogRead(_pin) * (3.3 / 4095.0); // read voltage from the analog pin
	// float ph = 3.3 * voltage;

	Serial.println(voltage);
	Serial.println(ph);

	_readings[0] += ph;
	_readings_count++;

	_last_reading = millis();
	return true;
}

Events::EventType Sensors::PhSensor::checkTriggers()
{
	Events::EventType event = Events::EventType::EMPTY;
	return event;
}