#include "Device/Device.h"
#include "Sensors/Sensors.h"
#include "ESP32WebServer/ESP32WebServer.h"

#include "Logger/Logger.h"
#include "Logger/Loggers/API.hpp"
#include "Logger/Loggers/Serial.hpp"

#include "Notification/Notification.h"
#include "Events/Events.h"

#include "PhSensor.h"

#include <Arduino.h>
#include <SPIFFS.h>
#include <CronAlarms.h>

int status = WL_IDLE_STATUS;

Sensors::PhSensor ph_sensor(35, 1, "WaterPh");

void initTasks();
void setupSensor();
void sendData();

void GmailNotification(
    const char *title,
    const char *message);

Logger logger = Logger("main");

void setup()
{
  Serial.begin(115200);

  Device::setup();

  Logger::addStream(Loggers::logToSerial);
  Logger::addStream(Loggers::logToAPI);

  Notification::addStream(GmailNotification);

  initTasks();
  setupSensor();

  logger.log("Setup complete");
  Notification::push("PhMeter-init", "Device started");
}

void loop()
{
  Cron.delay(); 
  Sensors::loop();

  Events::notifyListeners();
  sendData();
}

void initTasks()
{
  logger.log("Setting tasks...");
  Cron.create(
      "0 0 4 * * *",
      Device::setupTime,
      false);

  Cron.create(
      "0 */2 * * * *",
      WiFiManager::manageConnection,
      false);          

  Cron.create(
      "*/30 * * * * *",
      Device::sendHeartbeat,
      false);          
}

void sendData()
{
  if (Sensors::readings.size() > 0)
  {
    JsonVariant data = Sensors::readings.as<JsonVariant>();
    Device::device->postReadings(data, "WaterPh");
    ESP32WebServer::last_reading = data;
    Sensors::readings.clear();
  }
}

void setupSensor()
{
  logger.log("Setting sensors...");

  Config sensor_config = Config("sensor");
  sensor_config.load();

  ph_sensor.setSampling(
      sensor_config.data["sampling_amount"],
      sensor_config.data["sampling_interval"]);

  // thermometer.setTriggerValues(
  //     sensor_config.data["trigger_low"],
  //     sensor_config.data["trigger_high"]);
}

void GmailNotification(const char *title, const char *message)
{
  Device::device->postNotification(title, message);
}

// #include "DFRobot_ESP_PH.h"
// #include <EEPROM.h>

// DFRobot_ESP_PH ph;
// #define ESPADC 4096.0   //the esp Analog Digital Convertion value
// #define ESPVOLTAGE 3300 //the esp voltage supply value
// #define PH_PIN 35		//the esp gpio data pin number
// float voltage, phValue, temperature = 25;

// void setup()
// {
// 	Serial.begin(115200);
// 	EEPROM.begin(32);//needed to permit storage of calibration value in eeprom
// 	ph.begin();
// }

// void loop()
// {
// 	static unsigned long timepoint = millis();
// 	if (millis() - timepoint > 1000U) //time interval: 1s
// 	{
// 		timepoint = millis();
// 		//voltage = rawPinValue / esp32ADC * esp32Vin
// 		voltage = analogRead(PH_PIN) / ESPADC * ESPVOLTAGE; // read the voltage
// 		Serial.print("voltage:");
// 		Serial.println(voltage, 4);
		
// 		//temperature = readTemperature();  // read your temperature sensor to execute temperature compensation
// 		Serial.print("temperature:");
// 		Serial.print(temperature, 1);
// 		Serial.println("^C");

// 		phValue = ph.readPH(voltage, temperature); // convert voltage to pH with temperature compensation
// 		Serial.print("pH:");
// 		Serial.println(phValue, 4);
// 	}
// 	ph.calibration(voltage, temperature); // calibration process by Serail CMD
// }

// float readTemperature()
// {
// 	//add your code here to get the temperature from your temperature sensor
// }