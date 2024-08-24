// ili9xxx_init.h
#pragma once

#include "esphome/core/helpers.h"
#include <cinttypes>
#include "ili9xxx_defines.h"

namespace esphome {
namespace ili9xxx {

// clang-format off
static const uint8_t PROGMEM INITCMD_ILI9341[] = {
  0xEF, 0x03, 0x80, 0x02,
  0xCF, 0x00, 0xC1, 0x30,
  0xED, 0x64, 0x03, 0x12, 0x81,
  0xE8, 0x85, 0x00, 0x78,
  0xCB, 0x39, 0x2C, 0x00, 0x34, 0x02,
  0xF7, 0x20,
  0xEA, 0x00, 0x00,
  ILI9XXX_PWCTR1, 0x23,
  ILI9XXX_PWCTR2, 0x10,
  ILI9XXX_VMCTR1, 0x3e, 0x28,
  ILI9XXX_VMCTR2, 0x86,
  ILI9XXX_MADCTL, 0x48,
  ILI9XXX_VSCRSADD, 0x00,
  ILI9XXX_PIXFMT, 0x55,
  ILI9XXX_FRMCTR1, 0x00, 0x18,
  ILI9XXX_DFUNCTR, 0x08, 0x82, 0x27,
  0xF2, 0x00,
  ILI9XXX_GAMMASET, 0x01,
  ILI9XXX_GMCTRP1, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
  ILI9XXX_GMCTRN1, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
  ILI9XXX_SLPOUT, 0x80,
  ILI9XXX_DISPON, 0x80,
  0x00 // End of list
};

// clang-format on
}  // namespace ili9xxx
}  // namespace esphome
