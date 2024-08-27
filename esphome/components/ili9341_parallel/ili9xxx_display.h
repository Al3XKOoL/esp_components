#pragma once
#include "esphome/core/component.h"
#include "esphome/components/display/display_buffer.h"
#include "esphome/core/gpio.h"  // Añade esta línea
#include "ili9xxx_defines.h"

#define BUFFER_SIZE 32  // Ajusta este valor según la memoria disponible

namespace esphome {
namespace ili9xxx {

static const char *const TAG = "ili9341_parallel";

class ILI9341ParallelDisplay : public display::DisplayBuffer {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;
  void display();
  void update() override;
  void set_data_pins(GPIOPin *d0, GPIOPin *d1, GPIOPin *d2, GPIOPin *d3, GPIOPin *d4, GPIOPin *d5, GPIOPin *d6, GPIOPin *d7) {
    this->data_pins_[0] = d0;
    this->data_pins_[1] = d1;
    this->data_pins_[2] = d2;
    this->data_pins_[3] = d3;
    this->data_pins_[4] = d4;
    this->data_pins_[5] = d5;
    this->data_pins_[6] = d6;
    this->data_pins_[7] = d7;
  }
  void set_dc_pin(GPIOPin *dc_pin) { this->dc_pin_ = dc_pin; }
  void set_wr_pin(GPIOPin *wr_pin) { this->wr_pin_ = wr_pin; }
  void set_rd_pin(GPIOPin *rd_pin) { this->rd_pin_ = rd_pin; }
  void set_reset_pin(GPIOPin *reset_pin) { this->reset_pin_ = reset_pin; }
  void set_cs_pin(GPIOPin *cs_pin) { this->cs_pin_ = cs_pin; }

 protected:
  void draw_absolute_pixel_internal(int x, int y, Color color) override;
  void setup_pins_();
  void init_lcd_();
  void command(uint8_t value);
  void data(uint8_t value);
  void send_command(uint8_t command_byte, const uint8_t *data_bytes, uint8_t num_data_bytes);
  void reset_();
  void write_byte(uint8_t value);
  void write_array(const uint8_t *data, uint16_t length);

  GPIOPin *data_pins_[8];
  GPIOPin *dc_pin_;
  GPIOPin *wr_pin_;
  GPIOPin *rd_pin_;
  GPIOPin *reset_pin_{nullptr};
  GPIOPin *cs_pin_{nullptr};

  int16_t width_{0};   ///< Display width as modified by current rotation
  int16_t height_{0};  ///< Display height as modified by current rotation
  int16_t offset_x_{0};
  int16_t offset_y_{0};
  uint16_t x_low_{0};
  uint16_t y_low_{0};
  uint16_t x_high_{0};
  uint16_t y_high_{0};
  const uint8_t *palette_{};

  bool pre_invertcolors_ = false;
  display::ColorOrder color_order_{display::COLOR_ORDER_BGR};
  bool swap_xy_{};
  bool mirror_x_{};
  bool mirror_y_{};
};

}  // namespace ili9xxx
}  // namespace esphome
