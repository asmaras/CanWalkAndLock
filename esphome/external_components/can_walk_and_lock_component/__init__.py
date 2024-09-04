import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

can_walk_and_lock_component_ns = cg.esphome_ns.namespace('can_walk_and_lock_component')
CanWalkAndLockComponent = can_walk_and_lock_component_ns.class_('CanWalkAndLockComponent', cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(CanWalkAndLockComponent)
}).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)