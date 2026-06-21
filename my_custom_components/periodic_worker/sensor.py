import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import CONF_ID, STATE_CLASS_MEASUREMENT, UNIT_CENTIMETER, ICON_RULER

# 1. Define the namespace and bind your single unified class
periodic_worker_ns = cg.esphome_ns.namespace("periodic_worker")
PeriodicWorker = periodic_worker_ns.class_(
    "PeriodicWorker", sensor.Sensor, cg.PollingComponent
)

# 2. Build the unified schema directly onto your class type
CONFIG_SCHEMA = (
    sensor.sensor_schema(
        PeriodicWorker,
        unit_of_measurement=UNIT_CENTIMETER,
        icon=ICON_RULER,
        accuracy_decimals=0,
        state_class=STATE_CLASS_MEASUREMENT,
    )
    .extend(cv.polling_component_schema("5s"))  # Adds the 'update_interval' key
)

async def to_code(config):
    cg.add_library("Adafruit GFX Library", None)
    cg.add_library("Adafruit IS31FL3731", None)
    cg.add_library("Adafruit VL53L0X", None)
    
    # Instantiate the unified object under its explicit C++ class name
    var = cg.new_Pvariable(config[CONF_ID])
    
    # Register it into the central scheduler loop for setup() and update()
    await cg.register_component(var, config)
    
    # Register it into the Home Assistant sensor tracking engine
    await sensor.register_sensor(var, config)
