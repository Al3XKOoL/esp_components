#pragma once

#include "esphome.h"

namespace esphome {
namespace climate_solar {

class ClimateSolar : public esphome::Component, public esphome::climate::Climate {
 public:
  void setup() override {
    // Inicialización de sensores
    this->last_cycle_time_sensor_->publish_state(0.0);
    this->daily_active_time_sensor_->publish_state(0.0);
    this->daily_energy_consumption_sensor_->publish_state(0.0);

    // Inicializa el interruptor de la bomba si está definido
    if (this->pump_switch_) {
      this->pump_switch_->turn_off();  // Asegúrate de que la bomba esté apagada al inicio
    }
  }

  void control(const esphome::climate::ClimateCall &call) override {
    if (call.get_mode().has_value()) {
      esphome::climate::ClimateMode mode = *call.get_mode();
      if (mode == esphome::climate::CLIMATE_MODE_HEAT) {
        if (!this->bomba_activa) {
          if (this->temp_sun_->state < this->temp_max_ &&
              (this->temp_sun_->state - this->temp_watter_->state) >= this->diff_high_) {
            if (this->pump_switch_) {
              this->pump_switch_->turn_on();
            }
            this->bomba_activa = true;
            this->cycle_start_time_ = millis();
            ESP_LOGI("main", "Bomba encendida debido a la temperatura adecuada");
          }
        } else {
          if ((this->temp_output_->state - this->temp_watter_->state) < this->diff_mid_) {
            if (this->pump_switch_) {
              this->pump_switch_->turn_off();
            }
            this->bomba_activa = false;
            uint32_t cycle_time = millis() - this->cycle_start_time_;
            this->last_cycle_times_.push_back(cycle_time);
            if (this->last_cycle_times_.size() > 3) {
              this->last_cycle_times_.erase(this->last_cycle_times_.begin());
            }
            this->daily_active_time_ += cycle_time;
            ESP_LOGI("main", "Bomba apagada debido a temperatura inadecuada");
          }
        }
      } else {
        if (this->pump_switch_) {
          this->pump_switch_->turn_off();
        }
        this->bomba_activa = false;
        ESP_LOGI("main", "Bomba apagada porque el modo HEAT está desactivado");
      }
    }
  }

  esphome::climate::ClimateTraits traits() override {
    auto traits = esphome::climate::ClimateTraits();
    traits.set_supports_current_temperature(true);
    traits.set_supports_action(true);
    traits.set_supported_modes({esphome::climate::CLIMATE_MODE_HEAT, esphome::climate::CLIMATE_MODE_OFF});
    traits.set_visual_min_temperature(this->visual_min_temp_);
    traits.set_visual_max_temperature(this->visual_max_temp_);
    return traits;
  }

  void loop() override {
    // Lógica para actualizar la hora de funcionamiento diaria
    if (millis() - this->last_reset_time_ >= 86400000) {  // 24 horas
      this->daily_active_time_ = 0;
      this->last_reset_time_ = millis();
    }

    // Actualiza los sensores con los datos actuales
    this->last_cycle_time_sensor_->publish_state((this->last_cycle_times_.empty() ? 0 : (this->last_cycle_times_[0] + this->last_cycle_times_[1] + this->last_cycle_times_[2]) / 1000.0));
    this->daily_active_time_sensor_->publish_state(this->daily_active_time_ / 1000.0);
    this->daily_energy_consumption_sensor_->publish_state((this->daily_active_time_ / 3600000.0) * this->pump_power_);
  }

  // Métodos para configurar parámetros desde la interfaz
  void set_temp_max(float temp_max) { this->temp_max_ = temp_max; }
  void set_diff_high(float diff_high) { this->diff_high_ = diff_high; }
  void set_diff_mid(float diff_mid) { this->diff_mid_ = diff_mid; }
  void set_visual_min_temp(float visual_min_temp) { this->visual_min_temp_ = visual_min_temp; }
  void set_visual_max_temp(float visual_max_temp) { this->visual_max_temp_ = visual_max_temp; }
  void set_pump_power(float pump_power) { this->pump_power_ = pump_power; }
  void set_pump_switch(esphome::switch_::Switch *pump_switch) { this->pump_switch_ = pump_switch; }

  // Métodos para configurar los parámetros desde el YAML
  void set_parameters(esphome::sensor::Sensor *temp_sun, esphome::sensor::Sensor *temp_watter, esphome::sensor::Sensor *temp_output, float temp_max, float diff_high, float diff_mid, float visual_min_temp, float visual_max_temp, float pump_power, esphome::switch_::Switch *pump_switch) {
    this->temp_max_ = temp_max;
    this->diff_high_ = diff_high;
    this->temp_sun_ = temp_sun;
    this->temp_watter_ = temp_watter;
    this->temp_output_ = temp_output;
    this->diff_mid_ = diff_mid;
    this->visual_min_temp_ = visual_min_temp;
    this->visual_max_temp_ = visual_max_temp;
    this->pump_power_ = pump_power;
    this->pump_switch_ = pump_switch;
  }

 private:
  esphome::sensor::Sensor *temp_sun_;
  esphome::sensor::Sensor *temp_watter_;
  esphome::sensor::Sensor *temp_output_;
  float temp_max_;
  float diff_high_;
  float diff_mid_;
  float visual_min_temp_;
  float visual_max_temp_;
  float pump_power_;
  esphome::switch_::Switch *pump_switch_;  // Añadido para controlar la bomba
  bool bomba_activa = false;
  uint32_t cycle_start_time_;
  std::vector<uint32_t> last_cycle_times_;
  uint32_t daily_active_time_;
  uint32_t last_reset_time_;
  esphome::sensor::Sensor *last_cycle_time_sensor_;
  esphome::sensor::Sensor *daily_active_time_sensor_;
  esphome::sensor::Sensor *daily_energy_consumption_sensor_;
};

}  // namespace climate_solar
}  // namespace esphome
