import esphome.codegen as cg
import esphome.config_validation as cv

hardware_aes_ns = cg.new_ns("hardware_aes")
HardwareAESComponent = hardware_aes_ns.class_("HardwareAESComponent", cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(HardwareAESComponent),
    cv.Required("key"): cv.string_strict,
    cv.Required("hmac_key"): cv.string_strict,
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[cv.GenerateID()])
    await cg.register_component(var, config)

    cg.add(var.set_key_from_string(config["key"]))
    cg.add(var.set_hmac_key_from_string(config["hmac_key"]))
