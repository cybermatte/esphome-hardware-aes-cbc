import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor

hardware_aes_ns = cg.esphome_ns.namespace("hardware_aes")
HardwareAESComponent = hardware_aes_ns.class_("HardwareAESComponent", cg.Component)

lora_ns = cg.esphome_ns.namespace("lora")
LoraComponent = lora_ns.class_("LoraComponent", cg.Component)

encrypted_sender_ns = cg.new_ns("encrypted_sender")
EncryptedSenderComponent = encrypted_sender_ns.class_("EncryptedSenderComponent", cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(EncryptedSenderComponent),
    cv.Required("temperature_sensor"): cv.use_id(sensor.Sensor),
    cv.Required("humidity_sensor"): cv.use_id(sensor.Sensor),
    cv.Required("aes"): cv.use_id(HardwareAESComponent),
    cv.Required("lora"): cv.use_id(LoraComponent),
    cv.Optional("update_interval", default="60s"): cv.update_interval,
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[cv.GenerateID()])
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
