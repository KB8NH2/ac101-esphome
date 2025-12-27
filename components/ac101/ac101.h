#pragma once
#include "esphome/core/component.h"
#include "driver/i2c.h"

namespace esphome {
namespace ac101 {

class AC101 : public Component {
 public:
  void setup() override;
  void dump_config() override;

  bool init();
  void set_volume(uint8_t vol);

 protected:
  bool write_reg(uint8_t reg, uint16_t val);
  bool read_reg(uint8_t reg, uint16_t *val);
};

}  // namespace ac101
}  // namespace esphome
