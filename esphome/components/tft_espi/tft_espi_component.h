#pragma once

#include "esphome/core/component.h"
#include "TFT_eSPI.h"

namespace esphome {
namespace tft_e_spi {

class TFT_eSPIComponent : public esphome::Component, public esphome::display::DisplayBuffer {
 public:
  TFT_eSPIComponent();

  void setup() override;
  void update() override;

  void fill_screen(uint16_t color);
  void print(int x, int y, const char* text, int size, uint16_t color);

  void set_rotation(int rotation);

 private:
  TFT_eSPI tft_;  // El objeto TFT_eSPI
  int rotation_ = 0;
};

}  // namespace tft_e_spi
}  // namespace esphome
