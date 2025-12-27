#include "ac101.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ac101 {

static const char *const TAG = "ac101";

static const uint8_t AC101_ADDR = 0x1A;

enum AC101Register : uint8_t {
  CHIP_AUDIO_RS = 0x00,
  PLL_CTRL1 = 0x01,
  PLL_CTRL2 = 0x02,
  SYSCLK_CTRL = 0x03,
  MOD_CLK_ENA = 0x04,
  MOD_RST_CTRL = 0x05,
  I2S_SR_CTRL = 0x06,
  I2S1LCK_CTRL = 0x10,
  I2S1_SDOUT_CTRL = 0x11,
  I2S1_SDIN_CTRL = 0x12,
  I2S1_MXR_SRC = 0x13,
  ADC_SRC = 0x14,
  ADC_DIG_CTRL = 0x15,
  ADC_VOL_CTRL = 0x16,
  DAC_DIG_CTRL = 0x17,
  DAC_VOL_CTRL = 0x18,
  DAC_MXR_SRC = 0x19,
  DAC_MXR_GAIN = 0x1A,
  DAC_MXR_GAIN2 = 0x1B,
  OMIXER_DACA_CTRL = 0x1C,
  OMIXER_SR = 0x1D,
  OMIXER_BST1_CTRL = 0x1E,
  HPOUT_CTRL = 0x1F,
  SPKOUT_CTRL = 0x20
};

void AC101::setup() {
  ESP_LOGI(TAG, "Initializing AC101 codec...");
  if (!this->init()) {
    ESP_LOGE(TAG, "AC101 initialization failed");
  } else {
    ESP_LOGI(TAG, "AC101 initialized successfully");
  }
}

void AC101::dump_config() {
  ESP_LOGCONFIG(TAG, "AC101 Audio Codec");
}

bool AC101::write_reg(uint8_t reg, uint16_t val) {
  uint8_t data[2] = {(uint8_t)(val >> 8), (uint8_t)(val & 0xFF)};
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (AC101_ADDR << 1) | I2C_MASTER_WRITE, true);
  i2c_master_write_byte(cmd, reg, true);
  i2c_master_write(cmd, data, 2, true);
  i2c_master_stop(cmd);
  esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
  i2c_cmd_link_delete(cmd);
  return ret == ESP_OK;
}

bool AC101::read_reg(uint8_t reg, uint16_t *val) {
  uint8_t data[2];
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (AC101_ADDR << 1) | I2C_MASTER_WRITE, true);
  i2c_master_write_byte(cmd, reg, true);
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (AC101_ADDR << 1) | I2C_MASTER_READ, true);
  i2c_master_read(cmd, data, 2, I2C_MASTER_LAST_NACK);
  i2c_master_stop(cmd);
  esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
  i2c_cmd_link_delete(cmd);

  if (ret != ESP_OK)
    return false;

  *val = (data[0] << 8) | data[1];
  return true;
}

bool AC101::init() {
  this->write_reg(CHIP_AUDIO_RS, 0x123);

  this->write_reg(PLL_CTRL1, 0x014F);
  this->write_reg(PLL_CTRL2, 0x8600);
  this->write_reg(SYSCLK_CTRL, 0x8B08);
  this->write_reg(MOD_CLK_ENA, 0x800C);
  this->write_reg(MOD_RST_CTRL, 0x800C);

  this->write_reg(DAC_DIG_CTRL, 0x8000);
  this->write_reg(DAC_VOL_CTRL, 0xA0A0);

  this->write_reg(OMIXER_DACA_CTRL, 0xFF80);
  this->write_reg(SPKOUT_CTRL, 0xE880);

  return true;
}

void AC101::set_volume(uint8_t vol) {
  uint16_t v = (vol << 8) | vol;
  this->write_reg(DAC_VOL_CTRL, v);
}

}  // namespace ac101
}  // namespace esphome