import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import CONF_ID

# Reference external AES component
hardware_aes_ns = cg.esphome_ns.namespace("hardware_aes")
HardwareAESCBCComponent = hardware_aes_ns.class_("HardwareAESCBCComponent", cg.Component)

# Use generic Component for sx126x since internal class isn't exposed
SX126xComponent = cg.Component

# Define namespace for this component
encrypted_sender_ns = cg.esphome_ns.namespace("encrypted_sender")
EncryptedSenderComponent = encrypted_sender_ns.class_("EncryptedSenderComponent", cg.Component)

# Configuration schema
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(EncryptedSenderComponent),
    cv.Required("temperature_sensor"): cv.use_id(sensor.Sensor),
    cv.Required("humidity_sensor"): cv.use_id(sensor.Sensor),
    cv.Required("aes"): cv.use_id(HardwareAESCBCComponent),
    cv.Required("lora"): cv.use_id(SX126xComponent),
    cv.Optional("update_interval", default="60s"): cv.update_interval,
}).extend(cv.COMPONENT_SCHEMA)

# Code generation hook
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    temp = await cg.get_variable(config["temperature_sensor"])
    hum = await cg.get_variable(config["humidity_sensor"])
    aes = await cg.get_variable(config["aes"])
    lora = await cg.get_variable(config["lora"])

    cg.add(var.set_temperature_sensor(temp))
    cg.add(var.set_humidity_sensor(hum))
    cg.add(var.set_aes(aes))
    cg.add(var.set_lora(lora))
    cg.add(var.set_update_interval(config["update_interval"]))
