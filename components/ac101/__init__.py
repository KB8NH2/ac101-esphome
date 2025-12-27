import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import core

ac101_ns = cg.esphome_ns.namespace("ac101")
AC101 = ac101_ns.class_("AC101", cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(AC101),
})

async def to_code(config):
    var = cg.new_Pvariable(config[core.CONF_ID])
    await cg.register_component(var, config)
