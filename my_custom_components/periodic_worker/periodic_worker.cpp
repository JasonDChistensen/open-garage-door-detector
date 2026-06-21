#include "periodic_worker.h"
#include "esphome/core/log.h"

namespace esphome {
namespace periodic_worker {
    
using namespace esphome; //LOG_STR

#define SDA 5
#define SCL 6
#define FEATHER_WING_ADDRESS 0x74

static const char *const TAG = "periodic_worker.component";

Adafruit_VL53L0X lox = Adafruit_VL53L0X();
Adafruit_IS31FL3731_Wing ledmatrix = Adafruit_IS31FL3731_Wing(); 

// Match the constructor signature and link the base PollingComponent class 
PeriodicWorker::PeriodicWorker() : PollingComponent(0) { }



void PeriodicWorker::setup() { 
  //sudo usermod -a -G dialout <your_username>
  //sudo gtkterm -p /dev/ttyACM0 -s 9600
  ESP_LOGI(TAG, "Periodic tracker initialized successfully!");
  


  if(!Wire.begin(SDA, SCL))
  {
    ESP_LOGE(TAG, "Wire.begin failed!");
    this->Component::mark_failed(LOG_STR("Wire.begin failed!"));
    return;
  }
  else
  {
    ESP_LOGI(TAG, "Wire.begin success!");
  }
  
  // Set the hardware timeout to 100 milliseconds (default is often too aggressive)
  Wire.setTimeOut(100);
  
  //if (!ledmatrix.begin(FEATHER_WING_ADDRESS))
  if (!ledmatrix.begin(FEATHER_WING_ADDRESS, &Wire))
  {
     ESP_LOGE(TAG, "IS31 (Feathwing) not found!");
     //this->Component::mark_failed();
     this->Component::mark_failed(LOG_STR("IS31 (Feathwing) not found!"));
     return;
  }
  else
  {
    ESP_LOGI(TAG, "IS31 (Feathwing) found!");
  }

  ESP_LOGI(TAG, "VL53L0X (Time of Flight) test");
  if (!lox.begin()) {
      ESP_LOGE(TAG, "Failed to boot VL53L0X");
      this->Component::mark_failed(LOG_STR("Failed to boot VL53L0X"));
      return;
  }
  else
  {
    ESP_LOGI(TAG, "VL53L0X found!");    
  }
  ESP_LOGI(TAG, "setup() done");
}

void PeriodicWorker::update() {
  VL53L0X_RangingMeasurementData_t measure;

  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

  float distance_mm;
  int distance_cm;
  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
  
    distance_mm = measure.RangeMilliMeter;
  } else {
    distance_mm = 9999;
  }
  distance_cm = int(distance_mm/10.0);
  
  // 3. Convert integer to standard string and pad out leading zeros
  std::string digit_str = std::to_string(distance_cm);
  while (digit_str.length() < 3) {
      digit_str = "0" + digit_str;
  }
    
    
  // 5. Extract single characters from the string index
  char char1 = digit_str[0];
  char char2 = digit_str[1];
  char char3 = digit_str[2];
    
  //ESP_LOGI(TAG, std::to_string(this->distance_).c_str());
  const uint8_t brightness = 4; // Brightness level (0-255)
  const uint8_t bg_color = 0;    // Background off
  const uint8_t text_size = 1;   // Standard 5x7 pixel font ratio
  const int16_t y_pos = 0;       // Center row positioning alignment
    
    
  ledmatrix.drawChar(0,  y_pos, char1, brightness, bg_color, text_size);  // 100's digit
  ledmatrix.drawChar(5,  y_pos, char2, brightness, bg_color, text_size);  // 10's digit
  ledmatrix.drawChar(10, y_pos, char3, brightness, bg_color, text_size); // 1's digit

  // Expose the variable directly to Home Assistant
  // Publish state directly from this object instance
  this->publish_state(distance_cm);
  
}

void PeriodicWorker::dump_config() {
  using esphome::sensor::log_sensor;

  // 1. Logs the standard Home Assistant entity details
  LOG_SENSOR("", "Unified Periodic Tracker", this);

  // 2. Logs your custom component configuration parameter
  ESP_LOGCONFIG(TAG, "  Polling Update Interval: %u ms", this->get_update_interval());

}

}  // namespace periodic_worker
}  // namespace esphome
