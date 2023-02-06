#ifndef PhMeter_h
#define PhMeter_h

#include "../SmartHome-MicroContoler-Lib/Events/Events.h"
#include "../SmartHome-MicroContoler-Lib/Sensors/Sensor.h"
#include <Arduino.h>

#define ESPADC 4096.0   //the esp Analog Digital Convertion value
#define ESPVOLTAGE 3300 //the esp voltage supply value

namespace Sensors
{
    extern Measures measure[1];
    class PhSensor : public Sensor
    {
    public:
        PhSensor(
            uint8_t pin,
            uint8_t id_sensor,
            const char *name);
        bool makeReading();
        Events::EventType checkTriggers();

    private:
        uint8_t _pin;
    };
} // namespace Sensor
#endif