#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_IS31FL3731.h>
#include <Adafruit_VL53L0X.h>


namespace esphome {
namespace periodic_worker {

// Inherit from both PollingComponent and Sensor
// Inherit from Sensor first, then PollingComponent
class PeriodicWorker : public sensor::Sensor, public PollingComponent {
 public:
  PeriodicWorker();

  void setup() override;
  void update() override;
  void dump_config() override;

  // Explicitly set priority to ensure the scheduler hooks register it
  float get_setup_priority() const override { return esphome::setup_priority::DATA; }
  

 protected:
};

}  // namespace periodic_worker
}  // namespace esphome
